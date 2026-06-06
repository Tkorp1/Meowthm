TRACK_FILES = [
    "Track0.txt",
    "Track1.txt",
    "Track2.txt",
    "Track3.txt"
]


def check_track(filename):
    last_time = -1
    count = 0

    with open(filename, "r", encoding="utf-8") as f:
        for line_num, line in enumerate(f, start=1):

            line = line.strip()

            if not line:
                continue

            parts = line.split()

            # Tap
            if parts[0] == "0":

                if len(parts) != 2:
                    print(f"{filename} 第{line_num}行格式错误")
                    return False

                try:
                    current_time = float(parts[1])
                except:
                    print(f"{filename} 第{line_num}行节拍非法")
                    return False

            # Hold
            elif parts[0] == "1":

                if len(parts) != 3:
                    print(f"{filename} 第{line_num}行格式错误")
                    return False

                try:
                    start = float(parts[1])
                    end = float(parts[2])
                except:
                    print(f"{filename} 第{line_num}行节拍非法")
                    return False

                if end <= start:
                    print(
                        f"{filename} 第{line_num}行 "
                        f"Hold结束时间({end}) <= 开始时间({start})"
                    )
                    return False

                current_time = start

            else:
                print(f"{filename} 第{line_num}行类型只能为0或1")
                return False

            if current_time <= last_time:
                print(
                    f"{filename} 第{line_num}行时间顺序错误 "
                    f"({current_time} <= {last_time})"
                )
                return False

            last_time = current_time
            count += 1

    print(f"{filename}: √ ({count} Notes)")
    return True


total = 0

for track in TRACK_FILES:

    if not check_track(track):
        print("\n谱面检查失败")
        exit()

    with open(track, "r", encoding="utf-8") as f:
        total += sum(1 for line in f if line.strip())

print("\n谱面检查通过 ✓")
print(f"总Notes数: {total}")