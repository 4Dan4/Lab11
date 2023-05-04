#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>

const char *target_sequence; // глобальная переменная для хранения целевой последовательности

int search_file(const char *filepath, const struct stat *file_info, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) { // если это файл (а не директория)
        FILE *fp;
        char buffer[1024];
        fp = fopen(filepath, "rb"); // открываем файл для чтения в бинарном режиме
        if (fp == NULL) { // если не удалось открыть файл
            printf("Could not open file %s\n", filepath);
            return 1;
        }
        int found_sequence = 0; // флаг, указывающий, найдена ли целевая последовательность в файле
        size_t bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp); // читаем файл блоками по 1024 байта
        while (bytes_read > 0) { // продолжаем чтение, пока не достигнем конца файла
            for (size_t i = 0; i < bytes_read - 2; i++) { // проверяем каждую тройку байтов в прочитанном блоке
                if (buffer[i] == target_sequence[0] && buffer[i+1] == target_sequence[1] && buffer[i+2] == target_sequence[2]) {
                    found_sequence = 1; // если целевая последовательность найдена, устанавливаем флаг
                    break;
                }
            }
            if (found_sequence) break; // если последовательность найдена, выходим из цикла
            bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp); // читаем следующий блок данных
        }
        if (found_sequence) { // если последовательность найдена
            printf("%s\n", filepath); // выводим имя файла
        }
        fclose(fp); // закрываем файл
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // если количество аргументов неверно
        printf("Usage: %s directory target_sequence\n", argv[0]);
        return 1;
    }
    target_sequence = argv[2]; // сохраняем целевую последовательность в глобальной переменной
    int flags = FTW_PHYS | FTW_DEPTH; // флаги для функции ftw()
    if (ftw(argv[1], search_file, 10) == -1) { // рекурсивно обходим указанный каталог и его подкаталоги
        printf("Error searching directory\n");
        return 1;
    }
    return 0;
}