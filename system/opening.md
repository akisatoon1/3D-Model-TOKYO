```mermaid
flowchart TB
  Note["注: 矢印は処理の流れを示す"]

  Start["main(argc, argv)<br/>起動・引数解析（入力/出力/--size）"]
  Read["input::parseInput(path)<br/>点群テキストを読み込み、model::Gridに格納"]
  Open["opening(grid)<br/>開口処理の入口"]
  Erode["erode(grid)<br/>収縮：近傍走査で小さなノイズを除去"]
  Dilate["dilate(grid)<br/>膨張：形状を復元"]
  Write["outputGrid(grid, filename)<br/>結果を保存（属性はconvertAttrToInt）"]
  End["完了メッセージ出力"]

  Start --> Read --> Open --> Erode --> Dilate --> Write --> End
