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
    part_data = []
    for i in alldata:
        if i[3] == 1 and i[2] <= 50 and i[2] != -9999:
            part_data.append(i)
    return part_data

def create_Delaunay(xyList):
    Truedata = Delaunay(np.array(xyList))
    return Truedata

def print_vrml(using_point,using_point2,spicy,Outfilename):
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
            f.write(str(i[0]))
            f.write(", ")
            f.write(str(i[2]))
            f.write(", ")
            f.write(str(-i[1]))
            f.write("\n")

        for i in using_point2:
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

def main():
    inputfile()
    filename1 = sys.argv[1]
    filename2 = sys.argv[2]
    outputname = sys.argv[3]

    point_data1 = import_points(filename1)
    point_data2 = import_points(filename2)
    using_data = select_points(point_data1)

    xy_catalog = []

    for point1 in using_data:
        xy_catalog.append([point1[0],point1[1]])

    for point2 in point_data2:
        xy_catalog.append([point2[0],point2[1]])

    tri_scipy = create_Delaunay(xy_catalog)
    print_vrml(using_data,point_data2,tri_scipy,outputname)
    print("ファイル出力が終了しました")

if __name__ == "__main__":
    main()