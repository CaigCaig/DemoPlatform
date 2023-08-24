#include "terminal.h"
#include <string.h>
#include <stdarg.h>

static char tmpCommand[40];

//-----------------------------------------------------------------------------
// Microrl callbacks
//-----------------------------------------------------------------------------
extern "C" {

    //Configure terminal
    void configure_terminal() {
        pTerminal->configure();
    }

    //Print micrrl callback
    void print_terminal(const char *str) {
        pTerminal->print(str);
    }

    //Execute microrl callback
    int execute_terminal(int argc, const char * const * argv) {
        return pTerminal->execute(argc, argv);}

    //Sigint microrl callback
    void sigint_terminal (void) {
        pTerminal->sigint();}
    
    //Complete microrl callback
    char** completion_terminal (int argc, const char * const * argv) {
        return pTerminal->completion(argc, argv);}

}



//-----------------------------------------------------------------------------
// Terminal class
//-----------------------------------------------------------------------------

Terminal::Terminal():
    m_countPrintFunction(0),
    prl(&rl)
{
    memset(&f_print, 0, sizeof(f_print));
    memset(tmpCommand, 0, sizeof(tmpCommand));
}




//Configure microrl callbacks
void Terminal::configure()
{
  
    m_countPrintFunction = 0;
    prl = &rl;

    memset(&f_print, 0, sizeof(f_print));
    memset(tmpCommand, 0, sizeof(tmpCommand));
	
    microrl_init (prl, ::print_terminal);
    microrl_set_execute_callback (prl, ::execute_terminal);
    microrl_set_sigint_callback (prl, ::sigint_terminal);
    microrl_set_complete_callback (prl, ::completion_terminal);
}


void Terminal::connectCallback() {
    printeol();
    printll("Hello stranger. It is goot day for work :)");    
}


//Insert char to microrl
void Terminal::insert(int ch) {
    microrl_insert_char(prl, ch);
}


//Out message to all terminals
void Terminal::print(const char *str) {
    for (int i = 0; i < m_countPrintFunction; i++) {
        if (f_print[i] != 0) {
            f_print[i](str);
        }
    }
}


void Terminal::printl(const char * str) {
    print(str);
    print(EOL);
}

void Terminal::printll(const char * str) {
    print(str);
    print(EOL);
    print(EOL);
}


void Terminal::eol() {
    print(EOL);
}

void Terminal::printeol() {
    print(EOL);
}


//Execute microrl callbacks
int Terminal::execute(int argc, const char * const *argv) {
    return 0;
}

char* helloNull[] = {0};

//Comletion microrl callbacks
char ** Terminal::completion(int argc, const char * const * argv) {
    int i = 0;
    i++;
    return helloNull;
}

//Sigint microrl callbacks
void Terminal::sigint()
{
}


//Add external print function
void Terminal::addPrint(InterfacePrint print_func) {
    if (m_countPrintFunction < COUNT_TERMINAL_INTERFACE) {
        f_print[m_countPrintFunction] = print_func;
        m_countPrintFunction++;
    }
}





//-----------------------------------------------------------------------------
// Вывод команд в терминал
//-----------------------------------------------------------------------------

//Очистка экрана
void Terminal::clearScreen() {
    //Clear
    print("\033[2J");        
    //Move to up left corner
    print("\033[H");    
}


//Сдвинуть в указанную позицию
void Terminal::moveToPos(uint32_t h, uint32_t v)
{
    uint32_t sizeCommand = 0;
    getCommand_MoveToPos(tmpCommand, sizeCommand, h, v);
    print(tmpCommand);
}


//Сдвинуть в верхний левый угол
void Terminal::moveToUpLeftCorner() {
    //Move to up left corner
    print("\033[H");    
}


//Reset terminal to initial state 
void Terminal::reset() {
    //Сбросить настройки терминала
    print("\033c");
}

//Сбросить аттрибуты текста
void Terminal::resetAttribute() {
    print("\033[0m");
}

//Сохранить позицию курсора и атрибуты
void Terminal::saveCursorPos() {
    //Save cursor position and attributes 
    print("\0337");    
}

//Востановить позицию курсора и аттрибуты
void Terminal::restoreCursorPos() {
    //Restore cursor position and attributes 
    print("\0338");    
}

//Скрыть курсов
void Terminal::hideCursor() {
    print("\033[8m");
}

//Очистить строку
void Terminal::clearCurrentLine() {
    print("\033[2K");
}





void write(const char* src, int src_size, char* dst, uint32_t& size) {
    memcpy(&dst[size], src, src_size);
    size += src_size;
}


//Формирование команды для установки курсора в заданную позицию
uint32_t Terminal::getCommand_MoveToPos(
    char* res_command, uint32_t& res_size, uint32_t h, uint32_t v
)
{    
    //Esc[Line;ColumnH
    res_size = 0;
    
    write("\x1B[", 2, res_command, res_size);
    
        
    char* vStr = utoa(v);
    uint32_t sizeV = strlen(vStr);
    write(vStr, sizeV, res_command, res_size);
    
    write(";", 1, res_command, res_size);
    
    char* hStr = utoa(h);
    uint32_t sizeH = strlen(hStr);
    write(hStr, sizeH, res_command, res_size);

    write("H", 1, res_command, res_size);
    write("\0", 1, res_command, res_size); 

    return res_size;
}




//Формирование команды для сдвига курсора вверх на n позиций
uint32_t Terminal::getCommand_MoveUpNLines(
    char* res_command, uint32_t& res_size, uint32_t n)
{
    return 0;
}



//Формирование команды для сдвига курсора вниз на n позиций
uint32_t Terminal::getCommand_MoveDownNLines(
    char* res_command, uint32_t& res_size, uint32_t n)
{
    return 0;
}



//Формирование команды для сдвига курсора вправо на n позиций
uint32_t Terminal::getCommand_MoveRightNLines(
    char* res_command, uint32_t& res_size, uint32_t n)
{
    return 0;
}


//Формирование команды для сдвига курсора влево на n позиций
uint32_t Terminal::getCommand_MoveLeftNLines(
    char* res_command, uint32_t& res_size, uint32_t n)
{
    return 0;
}


//Формирование команды для установки цвета текста
uint32_t Terminal::getCommand_SetTextColor(char* res_command, uint32_t& res_size, TextColor color)
{
    //Esc[3ColorH
    res_size = 0;    
    write("\x1B[3", 3, res_command, res_size);
        
    char* vStr = utoa(color);
    write(vStr, 1, res_command, res_size);
    write("m", 1, res_command, res_size);   
    write("\0", 1, res_command, res_size); 
        
    return res_size;
}


//Формирование команды для установки аттрибута подчёркивания
uint32_t Terminal::getCommand_UnderlineText(char* res_command, uint32_t& res_size)
{
    res_size = 0;    
    write("\x1B[4m", 4, res_command, res_size);
    write("\0", 1, res_command, res_size); 
    return res_size;
}














//-----------------------------------------------------------------------------
// Класс реализации таблицы
//-----------------------------------------------------------------------------
Table::Table() {
}



//-----------------------------------------------------------------------------
// Вывод элементов таблицы
//-----------------------------------------------------------------------------

//Вывод сктроки в заданную ячейку
void Table::outToCell(uint8_t col, uint8_t row, const char* str) 
{
    //CellAttribute &cell = cells[col][row];
    //print(cell.pos_command);
    //print(cell.color_command);
    pTerminal->resetAttribute();
    
    //Выводим атрибуты
    CellAttribute &cell = cells[col][row];    
    print(cell.attributes);
    
    //Выводим строку
    print(str);
    
    //Выводим пробелы на до конца ячейки
    int32_t countSpace = m_width[col] - strlen(str) - 1; 
    for (uint8_t i = 0; i < countSpace; i++)
        print(" ");
    
    
}


//Вывод значения в заданную ячейку
void Table::outToCell(uint8_t col, uint8_t row, uint32_t val)  {
    outToCell(col, row, utoa(val));
}


//Выводим в нужную строку разделяющую линию
void Table::outCrossLine(uint8_t row) {    
    uint32_t line = m_y + row;
    pTerminal->moveToPos(m_x, line);
    printCrossLine(line);        
}



//Вывод внешней границы в строке
void Table::outExternalBorder() {
    
    uint32_t line = m_y;        
        
    printTopLine(line);        
    for (uint8_t r = 0; r < m_row; r++)
        printTextLine(line);    
    printBottomLine(line);                                              
        
}


//Печать статической части таблицы
void Table::outStatic() 
{
    pTerminal->saveCursorPos();
    outExternalBorder();
    pTerminal->restoreCursorPos();    
}


//Печатает в таблице счётчик ячеек
void Table::outCellNumber()
{
    uint32_t count = 0;
    for (uint8_t c = 0; c < m_col; c ++) {
        for (uint8_t r = 0; r < m_row; r++) {
            print(cells[c][r].attributes);            
            print(utoa(count));
            count ++;
        }
    }
}


void Table::startUpdate() {
    pTerminal->saveCursorPos();
}


void Table::endUpdate() {
    pTerminal->restoreCursorPos();
}

//-----------------------------------------------------------------------------
// Конфигурация талицы
//-----------------------------------------------------------------------------

//Задать геометрию таблицы 
void Table::setGeometry(uint8_t x, uint8_t y, uint8_t col, uint8_t row, uint32_t width, ...)
{   
    setPos(x, y);
    setSize(col, row);
    
    if ((m_col == 0) || (m_row == 0))
        return;
    
    //Читаем ширину колонок
    va_list ap;
    va_start(ap, width); 
    
    uint8_t c = 0;
    m_width[c] = width;    
    do {
        c++;
        m_width[c] = va_arg(ap, int32_t);
    } while (c < m_col);        
    
    va_end(ap);  
    
    resetAttributes();    
}


//Задать геометрию таблицы 
void Table::setGeometry(uint8_t x, uint8_t y, uint8_t col, uint8_t row, const uint32_t* width) 
{
    setPos(x, y);
    setSize(col, row);
    
    if ((m_col == 0) || (m_row == 0))
        return;
    
    for (int c = 0; c < m_col; c++)
        m_width[c] = width[c];
           
    resetAttributes();    
}


//Установить позицию таблицы
void Table::setPos(uint8_t x, uint8_t y) {
     m_x = x; m_y = y;
}

//Установить размеры таблицы
void Table::setSize(uint8_t col, uint8_t row) {
    m_col = col;
    m_row = row;
    
    if (m_col > MAX_COL)
        m_col = MAX_COL;
    
    if (m_row > MAX_ROW)
        m_row = MAX_ROW;
}


//Сброс аттрибутов для ячеек
void Table::resetAttributes()
{
    //Добавляем аттрибут для перемещения
    //курсора в начало каждой ячейки
    for (uint8_t c = 0; c < m_col; c ++) {
        for (uint8_t r = 0; r < m_row; r++) {
            clearCellAttributes(c, r);
            
            addCellPosAttribute(c, r);              
        }
    }
}


//-----------------------------------------------------------------------------
// Работа с аттрибутами
//-----------------------------------------------------------------------------

//Атрибуты - строка с управляющими командами для
//терминала VT100, которые выводятся в терминал
//перед выводом в ячейку (перемещение курсора
//на ячейку, установка цвета)

static char tmpAttr[20];
static uint32_t tmpAttrSize = 0; 

//Расчитать для начала ячейки позицию курсора 
void Table::addCellPosAttribute(uint8_t col, uint8_t row) {
         
    //Расчитываем позицию курсора для ячейки
    uint32_t y = m_y + 1;
    uint32_t x = m_x + 1;
    
    for (uint32_t c = 0; c < col; c++)
        x += m_width[c];
    
    x += col;
    y += row;
    
    //Формируем команду перемещения курсора в рассчитанную позицию
    pTerminal->getCommand_MoveToPos(tmpAttr, tmpAttrSize, x, y);    
    addCellAttribute(col, row, tmpAttr);        
}



//Добавить аттрибут цвета текста
void Table::addCellColorAttribute(uint8_t col, uint8_t row, TextColor color) {
    pTerminal->getCommand_SetTextColor(tmpAttr, tmpAttrSize, color); 
    addCellAttribute(col, row, tmpAttr);     
}


//Добавить аттрибут цвета текста
void Table::addCellUnderlineAttribute(uint8_t col, uint8_t row) {
    pTerminal->getCommand_UnderlineText(tmpAttr, tmpAttrSize); 
    addCellAttribute(col, row, tmpAttr);     
}


//Очистить аттрибуты ячейки 
void Table::clearCellAttributes(uint8_t col, uint8_t row) {          
    CellAttribute& cell = cells[col][row];
    
    cell.text = 0;
        
    char* attributes = cell.attributes;
    uint32_t attrSize = sizeof(cell.attributes);
    
    memset(attributes, 0, attrSize);
}


//Дописать аттрибут к ячейке
void Table::addCellAttribute(uint8_t col, uint8_t row, const char* attr) {
    CellAttribute& cell = cells[col][row];    
    
    uint8_t attrSize = strlen(cell.attributes);    
    char* dst = &cell.attributes[attrSize];
    
    attrSize = strlen(attr);
    memcpy(dst, attr, attrSize);
}



//-----------------------------------------------------------------------------
// Работа с выводом элементов рамки
//-----------------------------------------------------------------------------


//Символы псевдографики
//----------------------------------------
//─│┌┐└┘├┤┬┴┼═║╒╓╔╕╖╗╘╙╚╛╜╝╞╟╠╡╢╣╤╥╦╧╨╩╪╫╬
//----------------------------------------


//Печать верхней границы рамки
void Table::printTopLine(uint32_t& line) {
    printFrameLine(line, "┌", "┐", "┬", "─");
}

//Печать линии в таблице
void Table::printCrossLine(uint32_t& line) {
    printFrameLine(line, "├", "┤", "┼", "─");
}

//Печать рамки для отдельной строки
void Table::printTextLine(uint32_t& line) {
    printFrameLine(line, "│", "│", "│", " ");
}

//Печать нижней границы рамки таблицы
void Table::printBottomLine(uint32_t& line) {
    printFrameLine(line, "└", "┘", "┴", "─");
}


////Печать верхней границы рамки
//void Table::printTopLine(uint32_t& line) {
//    printFrameLine(line, "╔", "╗", "╤", "═");
//}

////Печать линии в таблице
//void Table::printCrossLine(uint32_t& line) {
//    printFrameLine(line, "╟", "╢", "┼", "─");
//}

////Печать рамки для отдельной строки
//void Table::printTextLine(uint32_t& line) {
//    printFrameLine(line, "║", "║", "│", " ");
//}

////Печать нижней границы рамки таблицы
//void Table::printBottomLine(uint32_t& line) {
//    printFrameLine(line, "╚", "╝", "╧", "═");
//}




//Выводит строку рамки таблицы
void Table::printFrameLine(uint32_t& line, 
    const char* lc, const char* rc, const char* cr, const char* ln)
{
    pTerminal->moveToPos(m_x, line);
    
    //Left corner
    print(lc);
    
    for (uint8_t c = 0; c < m_col; c++) 
    {
        //Print line charachter
        uint8_t width = 0;
        while (width < m_width[c]) {
            print(ln);
            width++;
        }
        if (c < (m_col - 1)) 
            //Print cross character
            print(cr);
        else
            //Print right corner
            print(rc);  
    }
    line++;
}




void Table::print(const char* str) {
    pTerminal->print(str);
}




//-----------------------------------------------------------------------------
// Преобразование числа в строку ANSI C через быстрое деление
//-----------------------------------------------------------------------------

struct divmod10_t
{
    uint32_t quot;
    uint8_t rem;
};



inline static divmod10_t divmodu10(uint32_t n)
{
    divmod10_t res;

    res.quot = n >> 1;
    res.quot += res.quot >> 1;
    res.quot += res.quot >> 4;
    res.quot += res.quot >> 8;
    res.quot += res.quot >> 16;
    uint32_t qq = res.quot;

    res.quot >>= 3;
    res.rem = uint8_t(n - ((res.quot << 1) + (qq & ~7ul)));
    if(res.rem > 9)
    {
        res.rem -= 10;
        res.quot++;
    }
    return res;
}


static char utoaBuffer[13];

//utoa fast div
char * utoa(uint32_t value)
{
    char* buffer = &utoaBuffer[1];
    buffer += 11;
    *--buffer = 0;
    do
    {
        divmod10_t res = divmodu10(value);
        *--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0);
    return buffer;
}




//utoa fast div
char * itoa(int32_t value)
{
    //Чёто неохото было гуглить. Мой
    //тупой вариант взятия модуля
    uint32_t unsignedValue = value;
    if (value < 0)
        unsignedValue = (uint32_t)(value - (2*value));
    
    char* unsignedStr = utoa(unsignedValue);
    
    //Добавляем знак минус в начало
    if (value < 0) {
        unsignedStr--;
        *unsignedStr = '-';       
    }
           
    return unsignedStr;
}














