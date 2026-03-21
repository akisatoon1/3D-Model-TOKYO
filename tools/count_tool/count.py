#!/usr/bin/env python3
import sys

def count_attributes(filename):
    """
    ファイルを読み込み、最後の列の属性値をカウントする
    1: Ground
    3: Building
    """
    ground_count = 0
    building_count = 0
    
    try:
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                
                parts = line.split()
                if len(parts) >= 4:
                    attribute = int(parts[3])
                    if attribute == 1:
                        ground_count += 1
                    elif attribute == 3:
                        building_count += 1
        
        print(f"Ground (1): {ground_count}")
        print(f"Building (3): {building_count}")
        print(f"Total: {ground_count + building_count}")
        
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python count.py <input_file>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    count_attributes(input_file)
