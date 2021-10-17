# Современные технологии программирования
Ковалев Максим Игоревич, гр. ИВ-923. Лабораторная работа #2.

## Цель
Разработать и покрыть тестами парсер простейших SQL выражений.

## Описание языка
Язык SQL, обрабатываемый программой, состоит из выражений:
* `CREATE TABLE {TableName} ({ColumnDef1}, ...);`
* `INSERT INTO {TableName} ({ColumnName1}, ...) VALUES ({Value1}, ...);`
* `SELECT {ColumnName} FROM {TableName} [WHERE {Expression}];`
* `DELETE FROM {TableName} [WHERE {Expression}];`
* `DROP TABLE {TableName};`

## Использование
```bash
$ ./SQLParser -i|--input {SQLFile} [-o|--output {JSONFile}]
```
Если программа запущена без выходного файла, то данные в формате JSON будут выведены прямо в консоль.

## Установка
При использовании `Unix Makefiles`:
```bash
mkdir build
cmake -S . -B build
cd build && make
```
Исполнительный файл `SQLParser` и тестировочный файл `SQLParser_test` будут находиться:
 * В директории `build` и директории `build/tests` соответственно при сборке на Unix-системе/MinGW;
 * В директории `build\<config>` и директории `build\tests\<config` соответственно (по умолчанию `<config>` = `Debug`) при сборке с помощью MSVC.