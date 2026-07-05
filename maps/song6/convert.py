import re

bpm = 180
beat_ms = 60000 / bpm

def parse_maidata(filepath):
    with open(filepath, 'r') as f:
        text = f.read()
    lines = text.strip().split('\n')
    # 忽略标题行等
    # 实际解析：以 {数字} 开头，后面是逗号分隔的键位列表
    pattern = r'\{(\d+)\}([^\{]*)'
    notes = []  # 存储 (绝对拍数, 轨道列表)
    abs_beat = 0.0
    for match in re.finditer(pattern, text):
        div = int(match.group(1))
        duration_beat = 4.0 / div  # 每个音符占的拍数
        keys_str = match.group(2).strip()
        if not keys_str:
            continue
        # 按逗号分割（注意可能包含空串）
        parts = keys_str.split(',')
        for part in parts:
            # 处理当前时值位置
            # 生成该位置上的音符
            if part:  # 非空拍
                # 处理斜杠或范围
                key_set = set()
                if '/' in part:
                    keys = part.split('/')
                    for k in keys:
                        k = k.strip().rstrip('b')  # 去掉可能的b
                        if k.isdigit():
                            key_set.add(int(k))
                elif '-' in part:
                    # 处理范围，如 2-6
                    start, end = map(int, part.split('-'))
                    for k in range(start, end+1):
                        key_set.add(k)
                else:
                    # 单个数字
                    k = part.strip().rstrip('b')
                    if k.isdigit():
                        key_set.add(int(k))
                # 映射到轨道 (1-4)
                track_set = set()
                for k in key_set:
                    track = ((k-1) % 4) + 1
                    track_set.add(track)
                for t in track_set:
                    notes.append((abs_beat, t))
            # 无论是否有音符，都增加时值
            abs_beat += duration_beat
    return notes

def write_tracks(notes, out_prefix='Track'):
    tracks = [[] for _ in range(4)]
    for beat, track in notes:
        time_ms = beat * beat_ms
        # 拍数从1开始，但存储时用 beat+1 便于阅读，实际程序需要绝对毫秒，我们存储节拍数还是毫秒？
        # 根据代码，MapParser 期望轨道文件中每个音符一行，第一个数字 0 表示 Tap，第二个数字是拍数（从1开始）
        # 注意：targetTimeCalculator 接受 beats 参数（从1开始），并且 beats-- 处理。
        # 因此我们存储拍数（保留至少3位小数）
        tracks[track-1].append((beat+1, 0))  # 0 表示 Tap
    for i in range(4):
        with open(f"{out_prefix}{i}.txt", 'w') as f:
            for beat, typ in tracks[i]:
                f.write(f"{typ} {beat:.3f}\n")

if __name__ == "__main__":
    notes = parse_maidata('maidata.txt')
    write_tracks(notes)
