import sys
import os

def process_data(x_min, y_min, x_max, y_max, input_path, clipped_output, residual_output):
    """
    指定された範囲(矩形)内の点を抽出し、ファイルを2つに分割する関数
    """
    # ファイル存在チェックを追加
    if not os.path.exists(input_path):
        print(f"エラー: 入力ファイル '{input_path}' が見つかりません。")
        print(f"カレントディレクトリ: {os.getcwd()}")
        print(f"指定された絶対パス: {os.path.abspath(input_path)}")
        return
    
    if not os.access(input_path, os.R_OK):
        print(f"エラー: 入力ファイル '{input_path}' の読み取り権限がありません。")
        return

    output_fragment = clipped_output # 範囲内のデータ
    output_original = residual_output # 範囲外(残り)のデータ

    # カウンター（処理結果表示用）
    count_fragment = 0
    count_original = 0

    try:
        with open(input_path, 'r', encoding='utf-8') as f_in, \
             open(output_fragment, 'w', encoding='utf-8') as f_frag, \
             open(output_original, 'w', encoding='utf-8') as f_orig:

            for line in f_in:
                # 行末の改行を除去し、空白で分割
                parts = line.strip().split()
                
                # 空行などをスキップ
                if not parts:
                    continue

                try:
                    # データ仕様に基づき座標を取得
                    # Column 0: X座標, Column 1: Y座標 と仮定
                    x = float(parts[0])
                    y = float(parts[1])

                    # 範囲判定ロジック
                    if (x_min <= x <= x_max) and (y_min <= y <= y_max):
                        f_frag.write(line)
                        count_fragment += 1
                    else:
                        f_orig.write(line)
                        count_original += 1

                except ValueError:
                    # 数値変換できない行（ヘッダー等）は一旦originalに書き込むか、
                    # エラーログに出してスキップする。ここではoriginalに残す。
                    f_orig.write(line)

        print(f"処理完了:")
        print(f" - {output_fragment} (抽出): {count_fragment} 行")
        print(f" - {output_original} (残留): {count_original} 行")

    except Exception as e:
        print(f"予期せぬエラーが発生しました: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    # 引数チェック
    if len(sys.argv) < 5:
        print("Usage: python extract.py <xmin> <ymin> <xmax> <ymax> [input_file] [clipped_output] [residual_output]")
        print("Example: python extract.py -7542.0 -36050.0 -7540.0 -36040.0 data.txt")
        sys.exit(1)

    # コマンドライン引数のパース
    try:
        p_xmin = float(sys.argv[1])
        p_ymin = float(sys.argv[2])
        p_xmax = float(sys.argv[3])
        p_ymax = float(sys.argv[4])
        
        # 第5引数があればそれを入力ファイル名とする。なければデフォルト値
        input_file = sys.argv[5] if len(sys.argv) > 5 else 'input_data.txt'

        clipped_output = sys.argv[6] if len(sys.argv) > 6 else 'clipped.dat'
        residual_output = sys.argv[7] if len(sys.argv) > 7 else 'residual.dat'

    except ValueError:
        print("エラー: 座標範囲には数値を指定してください。")
        sys.exit(1)

    # 実行
    process_data(p_xmin, p_ymin, p_xmax, p_ymax, input_file, clipped_output, residual_output)
