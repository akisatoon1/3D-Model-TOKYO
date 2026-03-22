# アルゴリズム（詳細設計）

このドキュメントは、本プロジェクトを C++（C++17）で実装するための詳細設計書の下書きである。
OpenCV の `minAreaRect` を使用する。出力は VRML（ASCII VRML97, `.wrl`）形式で、各建物を直方体ポリゴンとして出力する仕様に変更する。`ignored_buildings` は出力ファイルに含めず実行ログに記録する。

## 目的
- 属性値が 4（建物）である点群から連結成分を抽出し、各建物について以下を求める。
  - 輪郭点（建物以外に隣接するメンバ点）
  - 床高（floor_z）：輪郭に隣接する平地(attr==2)の z の平均
  - 上面高さ（top_z）：その building メンバの z の中央値
  - 最小外接矩形（rect）：輪郭点の xy から得る 4 頂点

※ 輪郭に隣接する平地が一つも無い建物は無視（ignored）し、そのメンバ点全体を `ignored_buildings` に記録する。

## 前提・入力仕様
- 入力ファイルはテキストで各行が: `x y z attr`（空白区切り）
- x,y は小数部分が0の整数（実装では `int` を使用）
- z は `double`（高さ）
- attr : 1=川, 2=平地, 3=高い木, 4=建物
- 隣接は 8 近傍（dx,dy ∈ {-1,0,1}, (0,0) 除く）

## 出力仕様（最終）
- 出力形式: VRML（ASCII VRML97, `.wrl`）。出力ファイルは `--out-vrml` で指定（デフォルト `out.wrl`）。

- 各 `Building` は基底の `rect`（4 頂点の XY）と `floor_z` / `top_z` から 8 頂点（直方体）を生成し、`IndexedFaceSet` でポリゴン（下面・上面・4 側面）を表現する。
- VRML に出力する座標マッピング（入力座標 -> VRML）:
  - 入力: (x_in, y_in, z_in)
  - VRML: (X, Y, Z) = (x_in, z_in, -y_in)
    - つまり X = x_in, Y = z_in（高さを VRML の Y 軸に対応）、Z = -y_in（Y 軸反転）

- 各直方体の頂点番号付け（内部規約）:
  - 0..3: bottom（floor_z）対応する rect の 4 頂点
  - 4..7: top（top_z）同じ XY に対応する上面

- coordIndex の例（各面は -1 で区切る）:
  - bottom: 0,1,2,3,-1
  - top:    4,7,6,5,-1
  - sides:  0,4,5,1,-1 ; 1,5,6,2,-1 ; 2,6,7,3,-1 ; 3,7,4,0,-1

- 色・マテリアルは固定（例: `diffuseColor 0.8 0.6 0.4`）。ignored 建物は VRML に含めず、実行ログへ記録する。

`algo` 側ではファイル先頭に `#VRML V2.0 utf8` を書き、建物ごとに `Shape { appearance Appearance { material Material { ... } } geometry IndexedFaceSet { ... } }` を連続して出力する。浮動小数点の桁数は CLI の `--precision` で制御する。


## 主要データ構造（ヘッダ草案）
以下は `include/model.h` 等に置く想定の構造体スケッチ。

```cpp
struct Point {
  int x;
  int y;
  double z;
  int attr; // 1..4
  bool visited = false;
};

struct Point3D { double x,y,z; };

struct Building {
  int id;
  // grid のセルを指すインデックス配列。各要素は (x,y) 座標のペアで表す。
  // 実装上は std::pair<int,int> か、行列インデックスを表す独自型を用いる。
  std::vector<std::pair<int,int>> members; // grid 内のセル位置の配列
  std::vector<std::pair<int,int>> contour; // members のサブセット（インデックスで参照）
  double floor_z = std::numeric_limits<double>::quiet_NaN();
  double top_z = std::numeric_limits<double>::quiet_NaN();
  std::array<std::pair<double,double>,4> rect; // rect の 4 頂点 (x,y)
};

struct Ignored {
  int id;
  std::string reason; // "no_adjacent_ground"
  std::vector<Point> points; // 値コピーして保存（x,y,z,attr）
};
```

Grid のヘッダ草案:

Grid 構造体は二次元配列（`std::vector<std::vector<Point>>`）を内部に持つ設計を想定する。以下は公開 API。

- void init(int xmin, int ymin, int width, int height)
  - 目的: グリッド領域を初期化し、`cells` を `height` 行 `width` 列の二次元配列として確保する。各セルは空（`attr==0`）で初期化する。

- bool inBounds(int x, int y) const
  - 目的: 指定座標がグリッド内に存在するかを判定する。

- Point* at(int x, int y)
  - 目的: 指定座標のセルが有効（`attr != 0`）ならその `Point*` を返す。範囲外または空セルなら `nullptr` を返す。

- Point& cellValue(int x, int y)
  - 目的: 指定座標のセルを参照で返す。空セルでも座標情報を含む `Point` を返す（呼び出し側で `attr` を確認する）。

- void setPoint(int x, int y, double z, int attr)
  - 目的: 指定座標に点データをセット（上書き）。

注: 実装では内部に `int x_min, y_min, width, height` と `std::vector<std::vector<Point>> cells` を持つ。

## 主な関数と API（擬似シグネチャ）
- bool parseInput(const std::string &path, Grid &grid);
- std::vector<Building> findBuildings(Grid &grid);
- void extractContour(Building &b, const Grid &grid);
- bool computeFloorZ(Building &b, const Grid &grid);
- void computeTopZ(Building &b);
- void computeRect(Building &b);
- void writeVrml(const std::string &path, const std::vector<Building>& buildings, const Meta& meta, int precision);

各関数の役割（簡潔）:
  - parseInput: 入力ファイル全行を解析して一時的に範囲（x_min,x_max,y_min,y_max）を求め、`Grid::init` で領域確保後に点を格納する。重複座標があればエラーを出力して直ちに実行を中止する。
- findBuildings: grid を二重ループで走査し `attr==4` かつ未訪問の点を起点に BFS/DFS で連結成分を抽出、Building を生成。
- extractContour: building.members を走査し各点の 8 近傍を `Grid::at` で参照して輪郭点を決定。
- computeFloorZ: 輪郭点の周囲（8 近傍）で `attr==2` の z を集め平均する。平地が一つも無ければ false を返す。
- computeTopZ: `members` に格納されたセル位置（x,y）のインデックスを使って各セルの z 値を取得し、その中央値を算出して `top_z` にセット。
- computeRect: contour の xy を `cv::Point2f` ベクタにして `cv::minAreaRect` を呼び、RotatedRect から 4 頂点を算出して格納。

## 処理フロー（高レベル）
1. CLI パース（`--input` `--out-vrml` 等）
2. parseInput -> grid を作る
3. findBuildings -> 建物候補リストを得る
4. 各 building について:
   a. extractContour
   b. computeFloorZ → false なら Ignored に移す（理由: "no_adjacent_ground"）
   c. computeTopZ
   d. computeRect
5. `writeVrml` で VRML ファイルに各 Building の直方体を順次書き出す（ignored はファイルに含めない）。
6. 実行時ログで ignored_buildings の要約を stdout に出力（id, reason, メンバ数）

## VRML 出力の注意
- VRML 出力はテキストストリーミングで行い、建物ごとに順次ファイルへ書き出すことでメモリ使用を抑える。
- ファイル先頭に `#VRML V2.0 utf8` を記述する。
- 各頂点は設計上の座標マップ `(X,Y,Z) = (x_in, z_in, -y_in)` に従って出力する。浮動小数点の小数桁は CLI の `--precision` で制御（`std::fixed` + `std::setprecision` を推奨）。
- 色・マテリアルは固定とし、Appearance/Material を各 Shape に付与する。
- ignored 建物は VRML に含めず、ログに id, reason, member_count を記録する。必要ならログにサンプル点を出すが、出力ファイルには含めない。
- 検証は MeshLab, FreeWRL 等のビューアで行い、面の向き（winding）が正しいか、座標変換で反転が無いかを確認する。

## CLI（想定）
- `--input <path>` (必須)
- `--out-vrml <path>` (default: `out.wrl`)
- `--precision <n>` (default: 6)
- `--fail-on-ignored` (ignored が存在したら非ゼロ終了)

## ログ・通知
- 実行中は簡潔なサマリを stdout に出力:
  - 読み込んだ点数、検出した建物数、ignored 建物数
- ignored_buildings の詳細（points を含む）は VRML に含めずログに保存する（実行ログまたは指定のログファイル）。

## エラー処理
- 入力パース失敗や OpenCV の例外はキャッチしてエラーメッセージを出力し非ゼロ終了。
- 重複座標は警告。既定: 最後の行を採用。オプションで挙動変更可能。

## ビルド/依存
- C++17
- OpenCV（>=3.0）
- 推奨: CMake プロジェクト設定

## マイルストーン（短期）
- M1: parseInput と grid 実装、簡易テスト
- M2: findBuildings と contour 抽出
- M3: floor_z/top_z と ignored ロジック
- M4: OpenCV 連携、minAreaRect、VRML 出力（`writeVrml` の実装）
- M5: CLI、ログ、追加テスト

---

この文書をベースに、次は実際のヘッダファイル（`include/model.h`）と主要関数の `.cpp` スケルトンを作成できます。進めますか？
