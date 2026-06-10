変更点:
ver2:現在のフォルダーでのファイル名を入力すればいい。
ver3:欠損点を属性値０、未判定点は属性値5で出力される



53394610と53394620は平野アリアなので、Ground_detect_plain.pyを使って判定してください。
ほかのアリアはGround_detect_city.pyを使ってください。



使い方：
py Ground_detect_plain(またはcity).py 入力ファイル　出力ファイル



実行例:
py Ground_detect_plain.py 53394621_dsm_1m.dat 53394621_output.txt
[INFO] reading: 53394621_dsm_1m.dat
saved in:  53394621_output.txt
