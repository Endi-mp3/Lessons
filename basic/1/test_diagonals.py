import os
import random
import subprocess
from pathlib import Path

# Укажи путь к своей программе (исполняемый файл)
PROGRAM = "./a.out"  # пример: "/home/user/sum_matrix"

# Директория для тестовых файлов
TMP_DIR = Path("/tmp/matrix_tests")
TMP_DIR.mkdir(parents=True, exist_ok=True)

random.seed(42)  # фиксируем сид для воспроизводимости


def write_matrix_to_file(matrix, name):
    """
    Записывает квадратную матрицу в файл формата:
    N
    row1
    row2
    ...
    Возвращает путь к файлу.
    """
    n = len(matrix)
    path = TMP_DIR / f"{name}.txt"
    with open(path, "w", encoding="utf-8") as f:
        f.write(str(n) + "\n")
        for row in matrix:
            f.write(" ".join(map(str, row)) + "\n")
    return str(path)


def sum_expected(matrix, mode):
    """
    Вычисляет ожидаемую сумму по режимам:
    1 — выше главной диагонали (j > i), без диагонали
    2 — ниже главной диагонали (i > j), без диагонали
    3 — выше побочной диагонали (i + j < n - 1), без диагонали (i + j == n - 1 исключить)
    4 — ниже побочной диагонали (i + j > n - 1), без диагонали
    """
    n = len(matrix)
    total = 0
    for i in range(n):
        for j in range(n):
            if mode == 1 and j > i:
                total += matrix[i][j]
            elif mode == 2 and i > j:
                total += matrix[i][j]
            elif mode == 3 and (i + j) < (n - 1):
                total += matrix[i][j]
            elif mode == 4 and (i + j) > (n - 1):
                total += matrix[i][j]
    return total


def run_program(param, filepath):
    """
    Запускает целевую программу и возвращает stdout как строку.
    """
    result = subprocess.check_output([PROGRAM, str(param), filepath], text=True)
    return result.strip()


def try_parse_int(s):
    try:
        return int(s)
    except ValueError:
        return None


def make_matrix(n, lo=0, hi=100):
    return [[random.randint(lo, hi) for _ in range(n)] for _ in range(n)]


def main():
    # Набор фиксированных тестов
    fixed_tests = [
        ("t01_1x1_zero", [[0]]),
        ("t02_1x1_val", [[7]]),
        ("t03_2x2_small", [[1, 2], [3, 4]]),
        ("t04_3x3_seq", [[1, 2, 3], [4, 5, 6], [7, 8, 9]]),
        ("t05_4x4_tens",
         [[10, 20, 30, 40],
          [50, 60, 70, 80],
          [90, 100, 110, 120],
          [130, 140, 150, 160]]),
        ("t06_3x3_diag_only", [[5, 0, 0], [0, 6, 0], [0, 0, 7]]),
        ("t07_5x5_edges", [
            [0, 1, 1, 1, 1],
            [1, 0, 1, 1, 1],
            [1, 1, 0, 1, 1],
            [1, 1, 1, 0, 1],
            [1, 1, 1, 1, 0],
        ]),
    ]

    # Сгенерированные случайные тесты (ещё 8 штук для итого 15+)
    random_tests = []
    sizes = [2, 3, 4, 5, 6, 7, 8, 9]
    for idx, n in enumerate(sizes, start=1):
        random_tests.append((f"rand_{idx}_{n}x{n}", make_matrix(n, lo=0, hi=65535)))

    tests = fixed_tests + random_tests

    print(f"Файлы будут сохранены в: {TMP_DIR}\n")

    total_runs = 0
    total_pass = 0
    total_fail = 0
    issues = []

    for name, matrix in tests:
        filepath = write_matrix_to_file(matrix, name)
        print(f"Тест: {name} | Файл: {filepath}")
        for mode in range(1, 5):
            expected = sum_expected(matrix, mode)
            try:
                output = run_program(mode, filepath)
                output_int = try_parse_int(output)
                total_runs += 1
                if output_int is not None and output_int == expected:
                    total_pass += 1
                    print(f"  Параметр {mode}: OK | Ожидалось: {expected} | Получено: {output_int}")
                else:
                    total_fail += 1
                    issues.append((name, mode, expected, output))
                    print(f"  Параметр {mode}: FAIL | Ожидалось: {expected} | Получено: '{output}'")
            except subprocess.CalledProcessError as e:
                total_fail += 1
                issues.append((name, mode, expected, f"error: {e}"))
                print(f"  Параметр {mode}: ERROR запуска программы: {e}")
        print()  # пустая строка между тестами

    print("РЕЗЮМЕ")
    print(f"- Всего запусков: {total_runs}")
    print(f"- Успешных: {total_pass}")
    print(f"- Неуспешных: {total_fail}")

    if issues:
        print("\nСписок проблемных кейсов:")
        for (name, mode, expected, got) in issues:
            print(f"- Тест '{name}', параметр {mode}: ожидалось {expected}, получено '{got}'")

    print(f"\nФайлы НЕ удаляются. Проверь: {TMP_DIR}")
    print("Можешь открыть любой *.txt и сверить матрицы руками, если нужно.")


if __name__ == "__main__":
    main()

