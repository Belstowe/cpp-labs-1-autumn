## Современные технологии программирования
Ковалев Максим Игоревич, гр. ИВ-923. Лабораторная работа #1.
<table>
  <tr>
    <th>#</th>
    <th>Структура проекта</th>
    <th>Использование библиотек</th>
    <th>Argparser</th>
  </tr>
  <tr>
    <td>9</td>
    <td>PFL MH ST</td>
    <td>FC</td>
    <td>cli11</td>
  </tr>
</table>

## Установка
```bash
mkdir build
cmake -S . -B build
cd build && make
```
Исполнительный файл `calc_app` и тестировочный файл `calc_test` будут находиться:
 * В директории `build` при сборке на Unix-системе;
 * В директории `build/<config>` (по умолчанию `<config>` = `Debug`) при сборке на Windows.