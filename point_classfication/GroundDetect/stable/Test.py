#ground detect ver 0 for test  2025  team.orange

import math

#step.0 ファイルイン　ver.1 GAN 25.10.19
def filein(fn):
    data = []
    with open(fn, 'r', encoding='utf-8', errors='ignore') as f:
        for s in f:
            t = s.split()
            x, y, z = float(t[0]), float(t[1]), float(t[2])
            if abs(z + 9999.99) <= 1e-3:
                continue
            data.append([x, y, z, None])
    return data

input_path= input("File name: ").strip()
print("[INFO] reading:", input_path)
data = filein(input_path)

#step.1 z座標一番低い基準点idx確定 ver.1:GAN 25.10.19
def find_std(data):
    mindis=10000
    minidx = None
    for i in range(len(data)):
        if data[i][2]<mindis:
            mindis = data[i][2]
            minidx = i
    return minidx

#use xy to get index ver.2 GAN 25.10.23
idx_by_xy={}
for i in range(len(data)):
    xi,yi=int(data[i][0]),int(data[i][1])
    idx_by_xy[(xi,yi)]=i

def idx(x,y):
    return idx_by_xy.get((int(x),int(y)),-1)


#tool.0.0 四近傍点idx ver.3 GAN 25.10.23
def c4(i):
    x,y=data[i][0],data[i][1]
    xi,yi = int(x),int(y)
    idx4s=[None]*4
    idx4s[0]=idx_by_xy.get((xi+1,yi))
    idx4s[1]=idx_by_xy.get((xi-1,yi))
    idx4s[2]=idx_by_xy.get((xi,yi+1))
    idx4s[3]=idx_by_xy.get((xi,yi-1))
    return idx4s

#tool.0.2 符号付相対高度 ver.1 GAN 25.10.19
def dis(i1,i2):
    return data[i2][2]-data[i1][2]

#属性値について、1は地面、3は建物

#tool.1 地面中心四近傍 ver.1 GAN 25.10.19
def ground(i):
    c=c4(i)
    for j in c:
        if j is None: continue
        if data[j][3] is not None:continue
        if(dis(i,j)>=4):data[j][3]=3#建物判定
        else : data[j][3]=1#地面判定

#tool.3 建物中心四近傍 ver.1 GAN 25.10.20
def building(i):
    c=c4(i)
    for j in c:
        if j is None: continue
        if data[j][3] is not None:continue
        if(dis(i,j)<=-2):data[j][3]=1#地面判定
        else:data[j][3]=3#建物判定

#main function ver.2 GAN 25.10.25
from collections import deque

zi = find_std(data) 
data[zi][3] = 1
q = deque([zi])
seen = [False]*len(data)

while True:
    if not q:
        nxt = next((i for i,row in enumerate(data) if row[3] is None and abs(row[2]-data[zi][2])<4),None)
        if nxt is None:
            break
        data[nxt][3] = 1
        q.append(nxt)

    u = q.popleft()
    if seen[u]:
        continue
    seen[u] =True
    if data[u][3]==1:
        ground(u)
        for i in c4(u):
            if i is not None and (not seen[i]) and data[i][3] in (1,3):q.append(i)
    if data[u][3]==3:
        building(u)
        for i in c4(u):
            if i is not None and (not seen[i]) and data[i][3] in (1,3):q.append(i)

import os
out_name = input("Please enter the Output file name: ").strip()
output_path=out_name if os.path.dirname(out_name) else os.path.join(os.path.dirname(input_path),out_name)

with open(output_path,"w",encoding="utf-8") as f:
    for x,y,z,cls in data:
        lab = 0 if cls is None else int(cls)
        f.write(f"{x} {y} {z} {lab}  \n")

print("saved in: ",output_path)