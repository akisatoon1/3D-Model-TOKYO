import sys
import matplotlib.pyplot as plt

GROUND = 1
RIVER = 2
BUILDING = 3
TREE = 4

TYPENAME_MAP = {
    GROUND: "ground",
    RIVER: "river",
    BUILDING: "building",
    TREE: "tree",
}

COLOR_MAP = {
    GROUND: "black",
    RIVER: "cyan",
    BUILDING: "red",
    TREE: "green",
}

def read_file(path, x_grounds, y_grounds, x_rivers, y_rivers, x_buildings, y_buildings, x_trees, y_trees):
    xmin, xmax = float("inf"), float("-inf")
    ymin, ymax = float("inf"), float("-inf")

    with open(path, "r") as f:
        for line in f:
            data = line.split()

            x = float(data[0])
            y = float(data[1])
            tp = int(data[3])

            xmin = min(xmin, x)
            xmax = max(xmax, x)
            ymin = min(ymin, y)
            ymax = max(ymax, y)

            if tp == GROUND:
                x_grounds.append(x)
                y_grounds.append(y)
            
            elif tp == RIVER:
                x_rivers.append(x)
                y_rivers.append(y)
            
            elif tp == BUILDING:
                x_buildings.append(x)
                y_buildings.append(y)
            
            elif tp == TREE:
                x_trees.append(x)
                y_trees.append(y)
            else:
                print("Unknown attibute:", tp)

    return xmin, xmax, ymin, ymax


def limit(xmin, xmax, ymin, ymax):
    plt.xlim(xmin - 1, xmax + 1)
    plt.ylim(ymin - 1, ymax + 1)


def set_graph_size(x_size, y_size):    
    # 縮尺する
    scale = 1000
    m2inch = 39.370
    width, height = x_size / scale * m2inch, y_size / scale * m2inch
    plt.figure(figsize=(width, height))


def scatter_points(x_ax, y_ax, type_):
    group = TYPENAME_MAP[type_]
    color = COLOR_MAP[type_]

    print(f"Plotting {group} points with color {color}...")

    plt.scatter(
        x_ax,
        y_ax,
        c=color,
        s=1,
        label=group
    )


def save(path):
    plt.xlabel("X")
    plt.ylabel("Y")

    plt.title("display ground detection")
    plt.grid()
    plt.legend()

    plt.savefig(path)


def main():
    x_grounds, y_grounds = [], []
    x_rivers, y_rivers = [], []
    x_buildings, y_buildings = [], []
    x_trees, y_trees = [], []

    print("Reading data from", sys.argv[1])
    xmin, xmax, ymin, ymax = read_file(sys.argv[1], x_grounds, y_grounds, x_rivers, y_rivers, x_buildings, y_buildings, x_trees, y_trees)
    print(f"bounding box: ({xmin}, {ymin}) to ({xmax}, {ymax})")

    limit(xmin, xmax, ymin, ymax)
    set_graph_size(xmax - xmin, ymax - ymin)

    scatter_points(x_grounds, y_grounds, GROUND)
    scatter_points(x_rivers, y_rivers, RIVER)
    scatter_points(x_buildings, y_buildings, BUILDING)
    scatter_points(x_trees, y_trees, TREE)

    save(sys.argv[2])

if __name__ == "__main__":
    main()
