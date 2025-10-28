import random
import subprocess
import os

def gen_number(length):
    """Генерация случайного числа заданной длины (без ведущих нулей)."""
    first_digit = str(random.randint(1, 9))
    other_digits = ''.join(str(random.randint(0, 9)) for _ in range(length - 1))
    return first_digit + other_digits

def write_file(fname, content):
    with open(fname, "w") as f:
        f.write(content + "\n")  # добавляем перевод строки

def read_file(fname):
    with open(fname, "r") as f:
        return f.read().strip()

def run_test(lenA, lenB):
    # 1. Генерация чисел
    A = gen_number(lenA)
    B = gen_number(lenB)

    # 2. Запись в файлы
    write_file("A.txt", A)
    write_file("B.txt", B)

    # 3. Запуск программы longmath_add
    subprocess.run(["./longmath_add", "A.txt", "B.txt", "Res.txt"], check=True)

    # 4. Чтение результата
    Res = read_file("Res.txt")

    # 5. Проверка
    ok = int(A) + int(B) == int(Res)
    print(f"Test A({lenA} digits) + B({lenB} digits):", "OK" if ok else "FAIL")

    if not ok:
        print("A:", A[:50], "...", len(A))
        print("B:", B[:50], "...", len(B))
        print("Res:", Res[:50], "...", len(Res))
        print("Expected:", str(int(A) + int(B))[:50], "...")

if __name__ == "__main__":
    # Несколько тестов разной длины
    run_test(50, 30)
    run_test(4096, 2048)
    run_test(2000, 2000)

