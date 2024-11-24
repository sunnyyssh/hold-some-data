import matplotlib.pyplot as plt
import math

def show_plot(file_name, color, label):
    s_real = 0.25 * math.pi + 1.25 * math.asin(0.8) - 1
    n_coords = []
    s_coords = []
    with open(file_name, encoding='utf-16') as f:
        for line in f.readlines():
            line = line.rstrip('\\n')
            n_str, f_str = line.split(';')[0:2]
            n_i, s_i = int(n_str.split(' = ')[1]), float(f_str.split(' = ')[1])
            n_coords.append(n_i)
            s_coords.append(s_i)
    plt.plot(n_coords, s_coords, marker='o', markersize=3, color=color, label=label)

plt.figure(figsize=(15, 10))
plt.title('Отклонение значения площади от ее точной оценки ΔS (%), в зависимости от количества точек N для двух масштабов прямоугольной области')
plt.xlabel('Ось N')
plt.ylabel('Ось ΔS, в процентах')
plt.axhline(y=0, color='black', linestyle='--', label=f'y = 0')
show_plot(r'wide_area.txt', 'blue', 'Генерация в широкой области')
show_plot(r'narrow_area.txt', 'red', 'Генерация в узкой области')
plt.legend(loc='lower right')
plt.savefig("circles_second.png", dpi=300)
plt.show()