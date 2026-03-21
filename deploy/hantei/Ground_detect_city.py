#ground detect ver 1  2025  team.orange

import math,os,sys

#step.0 ファイルイン　ver.1  25.10.19
def filein(fn):
    data = []
    with open(fn, 'r', encoding='utf-8', errors='ignore') as f:
        for s in f:
            t = s.split()
            x, y, z = float(t[0]), float(t[1]), float(t[2])
            if abs(z + 9999.99) <= 1e-3:
                data.append([x,y,z,0])
            else:
                data.append([x, y, z, None])
    return data

#tool.0.0 四近傍点idx ver.3  25.10.23
def c4(i):
    x,y=data[i][0],data[i][1]
    xi,yi = int(x),int(y)
    return [
        idx_by_xy.get((xi + 1, yi)),  #0右
        idx_by_xy.get((xi - 1, yi)),  #1左
        idx_by_xy.get((xi, yi + 1)),  #2上
        idx_by_xy.get((xi, yi - 1)),  #3下
    ]

#tool.0.0.1　ある近傍点の反対側の近傍点のidx確定 ver.2 25.10.23
def c4a(i, s):
    try:k=s.index(i)
    except ValueError:return None

    xj, yj = int(data[i][0]), int(data[i][1])
    if k == 0: return idx_by_xy.get((xj - 2, yj))#右→左
    elif k == 1:return idx_by_xy.get((xj + 2, yj))#左→右
    elif k == 2:return idx_by_xy.get((xj, yj - 2))#上→下
    elif k == 3:return idx_by_xy.get((xj, yj + 2))#下→上
    else:return None

#tool.0.1 符号付角度計算 ver.3  25.11.19
def degree(i1, i2, i3, eps=1e-12):
    if (i1 is None) or (i2 is None) or (i3 is None):return 0.0

    x1, y1, z1 = data[i1][0], data[i1][1], data[i1][2]
    x2, y2, z2 = data[i2][0], data[i2][1], data[i2][2]
    x3, y3, z3 = data[i3][0], data[i3][1], data[i3][2]
    d1 = math.hypot(x1 - x2, y1 - y2)
    d2 = math.hypot(x3 - x2, y3 - y2)
    if (d1 < eps) or (d2 < eps):return 0.0

    m1 = (z1 - z2) / d1
    m2 = (z3 - z2) / d2

    num = (m2 - m1)
    den = (1.0 + m1 * m2)
    ang = math.degrees(math.atan2(num, den))
    return ang

#tool.0.2 符号付相対高度 ver.1  25.10.19
def dis(i1,i2):
    return data[i1][2]-data[i2][2]

#step.2 基準値に最も近い未判定点idx確定 ver.3: 25.10.23
def find_std(data):
    mindis=10000
    minidx = None
    for i in range(len(data)):
        if (data[i][3] is None)and(abs(data[i][2]-zstd)<mindis)and(abs(data[i][2]-zstd)<4) :
            mindis = abs(data[i][2]-zstd)
            minidx = i
    return minidx

#属性値について、1は地面、2は川、3は建物、4は木
#tool.1 地面中心四近傍 ver.2  25.11.19
def ground_city(i):
    c = c4(i)
    for j in c:
        if j is None:continue
        if data[j][3] is not None:continue

        dj  = -dis(i, j)#相对高度差
        ang = -degree(j, i, c4a(j, c))#i,j,对侧点带符号夹角

        if (dj >= 6) or (ang > 42) :data[j][3] = 3 #建物
        else:data[j][3] = 1 #地面


def ground_plain(i):
    c = c4(i)
    for j in c:
        if j is None:continue
        if data[j][3] is not None:continue

        dj  = -dis(i, j)#相对高度差
        ang = -degree(j, i, c4a(j, c))#i,j,对侧点带符号夹角

        if (dj >= 6) or (ang > 55) :data[j][3] = 3 #建物
        else:data[j][3] = 1 #地面

#tool.2 川中心四近傍 ver.2  25.11.19
def river(i): 
    c = c4(i)
    for j in c:
        if j is None:continue
        if data[j][3] is not None:continue

        dj  = -dis(i, j)#相对高度差
        ang=-degree(j, i, c4a(j, c))

        if (dj>=2) or (abs(ang)>=2):data[j][3] = 1#地面判定
        else:data[j][3]=1#川判定
    
#tool.3 建物中心四近傍 ver.2  25.11.19
def building_city(i):
    c=c4(i)
    for j in c:
        if j is None:continue
        if data[j][3] is not None:continue

        dj  = -dis(i, j)#相对高度差
        ang = -degree(j, i, c4a(j, c))#i,j,对侧点带符号夹角

        if dj >= -6 or (ang > -45):data[j][3] = 3#建物
        else:data[j][3] = 1#地面

def building_plain(i):
    c = c4(i)
    for j in c:
        if j is None or data[j][3] is not None:continue

        dj = data[j][2] - data[i][2]
        ang = degree(i, j, c4a(j, c))

        if dj <= -6.0 or ang<-11:
            data[j][3] = 1   # ground
            continue
        else:
            data[j][3] = 3   # building
            continue

#tool.4 木中心四近傍 ver.1  25.10.19
def tree(i):
    c = c4(i)
    for j in c:
        if j is None:continue
        if data[j][3] is not None:continue

        dj  = -dis(i, j)#相对高度差
        if dj >= 2:data[j][3] = 3#建物
        elif 0 <= dj <= 2:data[j][3] = 4#木
        else:data[j][3] = 1 #地面

#main
#ファイル入力
while True:
    try:
        input_path = sys.argv[1]
        if not input_path:
            print("Empty file name.")
            continue
        if not os.path.isfile(input_path):
            print(f"Can not find file : {input_path}")
            continue

        print("[INFO] reading:", input_path)
        data = filein(input_path)

        if not data:
            print("Empty data.")
            continue
        break

    except FileNotFoundError:
        print(f"Can not find file : {input_path}")
        continue
    except UnicodeDecodeError as e:
        print(f"Unicode Error: {e}")
        continue
    except Exception as e:
        print(f"Unknown Error : {e}")
        sys.exit(1)


#use xy to get index ver.3  25.11.19
idx_by_xy={}
for i in range(len(data)):
    xi,yi=int(data[i][0]),int(data[i][1])
    idx_by_xy[(xi,yi)]=i


#step.1 基準値確定　ver.3: 25.11.17
s = [0]*301
for i in data:
    if (-10<=i[2]) and (i[2]<=20):
        idxz=int(round(i[2]*10)+100)
        s[idxz]+=1
zstd= (s.index((max(s)))/10.0)-10


#基準点探し
zvalue=10000
zidx=None
for i in range(len(data)):
    if data[i][3]==0:
        continue
    if abs(data[i][2]-zstd)<zvalue:
        zvalue=abs(data[i][2]-zstd)
        zidx=i  


#キュー構築
data[zidx][3]=1
from collections import deque
groundD = deque([zidx])
riverD= deque()
buildingD = deque()
treeD = deque()
groundSeen   = set()
riverSeen    = set()
buildingSeen = set()
treeSeen     = set()


#拡散関数 ver2: 2025.11.19
def spread(listx, functionx, type):
    if   type == 1: seen = groundSeen
    elif type == 2: seen = riverSeen
    elif type == 3: seen = buildingSeen
    elif type == 4: seen = treeSeen
    else:
        print("When spread , row[3]!=1,2,3,4")
        sys.exit()

    while listx:
        u = listx.popleft()
        if u is None:continue
        lbl = data[u][3]
        if lbl is None:continue

        if lbl != type:
            if lbl==0:continue
            if   lbl == 1:groundD.append(u)
            elif lbl == 2:riverD.append(u)
            elif lbl == 3:buildingD.append(u)
            elif lbl == 4:treeD.append(u)
            else:pass
            continue


        if u in seen:continue
        seen.add(u)

        functionx(u)
        for i in c4(u):
            if i is None:continue
            li = data[i][3]
            if li is None:continue

            if   li == type and i not in seen:listx.append(i)
            elif li == 1 and i not in groundSeen:groundD.append(i)
            elif li == 2 and i not in riverSeen:riverD.append(i)
            elif li == 3 and i not in buildingSeen:buildingD.append(i)
            elif li == 4 and i not in treeSeen:treeD.append(i)


#拡散循環
while any(row[3] is None for row in data):
    if len(groundD) == 0:
        seed = find_std(data)
        if seed is None:
            print("Need to pick up a new unlabeled point.")
            sys.exit(0)
        if data[seed][3] is None:
            data[seed][3]=1
        groundD.append(seed)

    spread(groundD,   ground_city,   1)
    spread(buildingD, building_city, 3)
    spread(treeD,     tree,     4)
    spread(riverD,    river,    2)

#出力
output_path=sys.argv[2]

with open(output_path,"w",encoding="utf-8") as f:
    for x,y,z,cls in data:
        lab = 5 if cls is None else int(cls)
        f.write(f"{x} {y} {z} {lab}  \n")

print("saved in: ",output_path)