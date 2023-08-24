#ifndef __TERMINAL_H
#define __TERMINAL_H


#include "stm32f4xx.h"
#include "microrl.h"

#define EOL "\n\r"
#define COUNT_TERMINAL_INTERFACE  2

//Function for print on concrete interface
typedef void (*InterfacePrint)(const char * str);

typedef struct {
    InterfacePrint print;
    char* description[10];
} TerminalInterface;




//-----------------------------------------------------------------------------
// Базовый класс терминала
//-----------------------------------------------------------------------------

typedef enum { 
    tc_Black = 0x0, tc_Red, tc_Green, tc_Yello, tc_Blue, tc_Magenta, tc_Cyan, tc_White
} TextColor;


class Terminal
{
public:
    Terminal();

    virtual void connectCallback();

    void addPrint(InterfacePrint print_func);
    void clearScreen();   
    void moveToPos(uint32_t h, uint32_t v);
    void moveToUpLeftCorner();
    void reset();
    void resetAttribute();
    void saveCursorPos();
    void restoreCursorPos();
    void hideCursor();
    void clearCurrentLine();
    

    //Формирование команд терминала
    uint32_t getCommand_MoveToPos(char* res_command, uint32_t& res_size, uint32_t h, uint32_t v);
    uint32_t getCommand_MoveUpNLines(char* res_command, uint32_t& res_size, uint32_t n);
    uint32_t getCommand_MoveDownNLines(char* res_command, uint32_t& res_size, uint32_t n);
    uint32_t getCommand_MoveRightNLines(char* res_command, uint32_t& res_size, uint32_t n);
    uint32_t getCommand_MoveLeftNLines(char* res_command, uint32_t& res_size, uint32_t n);
    uint32_t getCommand_SetTextColor(char* res_command, uint32_t& res_size, TextColor color);
    uint32_t getCommand_UnderlineText(char* res_command, uint32_t& res_size);

public:
    virtual void configure();
    void insert(int ch);
    void print(const char * str);
    void printl(const char * str);
    void printll(const char * str);
    void eol();
    void printeol();
    
    virtual int execute(int argc, const char * const * argv);
    virtual char ** completion(int argc, const char * const * argv);
    virtual void sigint (void);
    
    
private:
    InterfacePrint f_print[COUNT_TERMINAL_INTERFACE];
    int m_countPrintFunction;

    microrl_t rl;
    microrl_t * prl;
};





//-----------------------------------------------------------------------------
// Класс таблицы
//-----------------------------------------------------------------------------    
#define MAX_COL 8
#define MAX_ROW 10    
    
class Table {
public:
    Table();
    
    void outToCell(uint8_t col, uint8_t row, const char* str);
    void outToCell(uint8_t col, uint8_t row, uint32_t val);
    void outCellNumber();
    virtual void outStatic();
    void outExternalBorder();    
    void outCrossLine(uint8_t row);

    void startUpdate();
    void endUpdate();

    //virtual void refreshData();
    
    void setGeometry(uint8_t x, uint8_t y, uint8_t col, uint8_t row, uint32_t width, ...);
    void setGeometry(uint8_t x, uint8_t y, uint8_t col, uint8_t row, const uint32_t* width);

private:
    void setPos(uint8_t x, uint8_t y);
    void setSize(uint8_t col, uint8_t row);
    void resetAttributes();

public:
    void addCellPosAttribute(uint8_t col, uint8_t row);
    void addCellColorAttribute(uint8_t col, uint8_t row, TextColor color);
    void addCellUnderlineAttribute(uint8_t col, uint8_t row);
    inline void clearCellAttributes(uint8_t col, uint8_t row);

protected:
    inline void addCellAttribute(uint8_t col, uint8_t row, const char* attr);

private:
    inline void printTopLine(uint32_t& line);
    inline void printCrossLine(uint32_t& line);
    inline void printTextLine(uint32_t& line);
    inline void printBottomLine(uint32_t& line);

    void printFrameLine(uint32_t& line, 
        const char* lc, const char* rc,
        const char* cr, const char* ln);
            
    inline void print(const char* str);    

private:
    uint8_t m_x, m_y;
    uint8_t m_col, m_row;    
    uint8_t m_width[MAX_COL];    

    typedef struct {
        char    attributes[40]; 
        char*   text;
    } CellAttribute;
    
    CellAttribute cells[MAX_COL][MAX_ROW];    
}; 







extern Terminal* pTerminal;
char * utoa(uint32_t value);
char * itoa(int32_t value);



#endif // __TERMINAL_H
