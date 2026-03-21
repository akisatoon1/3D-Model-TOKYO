from scipy.spatial import Delaunay
import numpy as np
import sys

def inputfile():
    if len(sys.argv) != 4:
        print("使用法: python3 delaunay02.py <入力ファイル名> <建物の地面ファイル名> <出力ファイル名>")
        sys.exit(1)

def import_points(name):
    data = np.loadtxt(name)
    return data

def select_points(alldata):
    point_num = 0
    part_data = []
    for i in alldata:
        if (i[3] == 1 or i[3] == 2) and i[2] <= 15 and i[2] != -9999:
            part_data.append(i)
            point_num += 1
    return part_data,point_num

def delete_points(usedata):
    """4点のうちの1点のみを使用する"""
    if len(usedata) == 0:
        return []
    
    # 高速検索のため、(x, y)座標をキーとする辞書を作成
    point_dict = {(point[0], point[1]): point for point in usedata}
    
    main_point = []
    for i, point in enumerate(usedata):
        # 偶数インデックスのみ処理
        if i % 2 == 0:
            x, y = point[0], point[1]
            # 隣接する4点が存在するかチェック
            if ((x+1, y) in point_dict and 
                (x, y+1) in point_dict and 
                (x+1, y+1) in point_dict):
                main_point.append(point)
            else:
                # 存在する点だけを追加
                main_point.append(point)
                if (x+1, y) in point_dict:
                    main_point.append(point_dict[(x+1, y)])
                if (x, y+1) in point_dict:
                    main_point.append(point_dict[(x, y+1)])
                if (x+1, y+1) in point_dict:
                    main_point.append(point_dict[(x+1, y+1)])

    return main_point

def create_Delaunay(xyList):
    Truedata = Delaunay(np.array(xyList))
    return Truedata

def print_vrml(using_point,using_point2,spicy,Outfilename):
    all_num = 0
    with open(Outfilename, "w", encoding="utf-8") as f:

        f.write("#VRML V2.0 utf8\n")
        f.write("\n")
        f.write("Shape{\n")
        f.write(" appearance Appearance {\n")
        f.write("  material Material { diffuseColor 0.1 0.7 0.2 }\n")
        f.write(" }\n")
        f.write(" geometry IndexedFaceSet {\n")
        f.write("   solid FALSE\n")
        f.write("    coord Coordinate {\n")
        f.write("     point [\n")

        for i in using_point:
            all_num += 1
            f.write(str(i[0]))
            f.write(", ")
            f.write(str(i[2]))
            f.write(", ")
            f.write(str(-i[1]))
            f.write("\n")

        for i in using_point2:
            all_num += 1
            f.write(str(i[0]))
            f.write(", ")
            f.write(str(i[2]))
            f.write(", ")
            f.write(str(-i[1]))
            f.write("\n")

        f.write(" ]\n")
        f.write("}\n")
        f.write("coordIndex [\n")

        for i in spicy.simplices:
            f.write(str(i[0]))
            f.write(", ")
            f.write(str(i[1]))
            f.write(", ")
            f.write(str(i[2]))
            f.write(", ")
            f.write("-1,\n")
            
        f.write("]\n")
        f.write("}\n")
        f.write("}\n")
        f.write("\n")
    return all_num

def main():
    inputfile()
    filename1 = sys.argv[1]
    filename2 = sys.argv[2]
    outputname = sys.argv[3]

    before_num = 0
    after_num = 0

    point_data1 = import_points(filename1)
    point_data2 = import_points(filename2)
    using_data,before_num = select_points(point_data1)
    del_data = delete_points(using_data)

    xy_catalog = []

    for point1 in del_data:
        xy_catalog.append([point1[0],point1[1]])

    for point2 in point_data2:
        xy_catalog.append([point2[0],point2[1]])

    tri_scipy = create_Delaunay(xy_catalog)
    after_num = print_vrml(del_data,point_data2,tri_scipy,outputname)
    print("削減前の点数:",before_num)
    print("削減後の点数:",after_num)
    print("削減率:",(after_num/before_num)*100,"%")
    print("ファイル出力が終了しました")

if __name__ == "__main__":
    main()