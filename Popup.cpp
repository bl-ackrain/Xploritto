//#include "globals.h"
#include "Popup.h"
#include "Pop.h"
#include "Icons.h"

namespace Popup
{

    /*Keyboard */

const char keyboardLayoutPage0[] = "1234567890qwertyuiop[]asdfghjkl;'\\zxcvbnm,./-=AB";
const char keyboardLayoutPage1[] = "!@*$%^&*()QWERTYUIOP{}ASDFGHJKL:\"|ZXCVBNM<>?_+ab";

const char* keyboardLayout[] = {
	keyboardLayoutPage0,
	keyboardLayoutPage1,
};

const uint8_t keyboardLayoutPages = 2;

const uint8_t keyboardYOffset = 44;
const uint8_t keysYOffset = keyboardYOffset+35;
const uint8_t keyboardXOffset = 14;

char keyboardGetChar(int8_t x, int8_t y, const char* layout) {
	if (y == 4) {
		if (x < 8 && x >= 3) {
			return ' ';
		}
		return 0;
	}
	if (y == 0 && x >= 10) {
		return 0;
	}
	if (y > 0) { // skip the backspace button
		x -= 2;
	}
	return layout[12*y + x];
}

void keyboardDrawKey(uint8_t x, uint8_t y, char c, uint8_t bc, uint8_t fc) {
    Pokitto::Display::setColor(bc);
    Pokitto::Display::fillRoundRect(x+1, y+1, 14, 18, 2);

	Pokitto::Display::setCursor(x+3, y+2);
    Pokitto::Display::setColor(fc);
	Pokitto::Display::print(c);
}

void keyboardDrawSwitch(const char* txt, uint8_t bc, uint8_t fc) {
	Pokitto::Display::setColor(bc);
	Pokitto::Display::fillRoundRect(keyboardXOffset+1, keysYOffset + 1 + 4*19, 31+15, 19, 2);
	Pokitto::Display::setCursor(keyboardXOffset+14, keysYOffset + 3 + 4*19);
    Pokitto::Display::setColor(fc);
	Pokitto::Display::print(txt);
}

void keyboardDrawLayout(const char* layout) {
	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 12; x++) {
			if (y == 0 && x >= 10) {
				// we skip the backspace key
				break;
			}
			keyboardDrawKey(keyboardXOffset+x*16, y*19 + keysYOffset, *(layout++), WHITE ,BLACK);
		}
	}
	// last we draw the new layout switch
	keyboardDrawSwitch(layout, 1, 2);
}

void keyboardDrawBackspace(uint8_t bc, uint8_t fc) {
    Pokitto::Display::setColor(bc);
	Pokitto::Display::fillRoundRect(keyboardXOffset+16*10+1, keysYOffset + 1, 31, 18, 2);
	Pokitto::Display::setCursor(keyboardXOffset+16*10+10, keysYOffset + 3);
    Pokitto::Display::setColor(fc);
	Pokitto::Display::print('\33');

}



void keyboardDrawSelect(uint8_t bc, uint8_t fc) {
    Pokitto::Display::setColor(bc);
	Pokitto::Display::fillRoundRect(keyboardXOffset+16*10+1, keysYOffset + 1 + 4*19, 31, 18, 2);

	Pokitto::Display::setCursor(keyboardXOffset+16*10+10, keysYOffset + 3 + 4*19);
    Pokitto::Display::setColor(fc);
	Pokitto::Display::print('\32');

}

void keyboardDrawExit(uint8_t bc, uint8_t fc) {
    Pokitto::Display::setColor(bc);
	Pokitto::Display::fillRoundRect(keyboardXOffset+16*8+1, keysYOffset + 1 + 4*19, 30, 18, 2);

	Pokitto::Display::setCursor(keyboardXOffset+16*8+10, keysYOffset + 2 + 4*19);
    Pokitto::Display::setColor(fc);
	Pokitto::Display::print('x');

}

void keyboardDrawSpace(uint8_t bc) {
    Pokitto::Display::setColor(bc);
	Pokitto::Display::fillRoundRect(keyboardXOffset+16*3+1, keysYOffset + 1 + 4*19, 110-32, 18, 2);
}

void keyboardDrawCursorReal(size_t x, size_t y, uint8_t curLayout, bool selected) {

	if (y == 0 && x >= 10) {
		// backspace
		keyboardDrawBackspace(selected?LIGHT_BLUE:DARK_GRAY, selected?WHITE:LIGHT_GRAY);
		return;
	}
	if (y == 4) {
		// lower row
		if (x < 3) {
			// switch
			keyboardDrawSwitch(&keyboardLayout[curLayout][46], selected?LIGHT_BLUE:DARK_GRAY, selected?WHITE:LIGHT_GRAY);
			return;
		}
		if (x < 8) {
			// space

			keyboardDrawSpace(selected?LIGHT_BLUE:DARK_GRAY);
			return;
		}
		if (x < 10) {
			// space

			keyboardDrawExit(selected?LIGHT_BLUE:RED, WHITE);
			return;
		}
		// select

		keyboardDrawSelect(selected?LIGHT_BLUE:GREEN, WHITE);
		return;
	}

	char c = keyboardGetChar(x, y, keyboardLayout[curLayout]);
	keyboardDrawKey(keyboardXOffset+x*16, y*19 + keysYOffset, c, selected?LIGHT_BLUE:WHITE, selected?WHITE:BLACK);
}

void keyboardDrawCursor(int8_t x, int8_t y, uint8_t curLayout) {

	keyboardDrawCursorReal(x, y, curLayout, true);
}

void keyboardEraseCursor(int8_t x, int8_t y, uint8_t curLayout) {

	keyboardDrawCursorReal(x, y , curLayout, false);
}



void keyboard(std::string title, char *text, size_t length)
{

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRect(0, keyboardYOffset, Pokitto::Display::width, 132 );

	Pokitto::Display::fillRect(keyboardXOffset, keyboardYOffset+1, Pokitto::Display::width-keyboardXOffset-4, 19);
	Pokitto::Display::setCursor(keyboardXOffset+3, keyboardYOffset+2);
    Pokitto::Display::setColor(WHITE);

	Pokitto::Display::print(title.c_str());
    Pokitto::Display::fontSize=2;

	keyboardDrawBackspace(DARK_GRAY,LIGHT_GRAY);
	keyboardDrawSpace(DARK_GRAY);
	keyboardDrawExit(RED, WHITE);
	keyboardDrawSelect(GREEN,WHITE);

	uint8_t curLayout = 0;

	keyboardDrawLayout(keyboardLayout[curLayout]);

	int8_t cursorX = 6;
	int8_t cursorY = 2;
	int8_t activeChar = strlen(text);

	while(1) {
		if(!Pokitto::Core::update())
		    continue;
		// update cursor movement
		int8_t cursorXPrev = cursorX;
		int8_t cursorYPrev = cursorY;
		keyboardEraseCursor(cursorX, cursorY, curLayout);
		// cursorX movement
		if (Pokitto::Buttons::repeat(BTN_LEFT, 5)) {
			if ((cursorY == 0 || cursorY == 4) && cursorX >= 10) {
				cursorX = 9;
            } else if (cursorY == 4 && cursorX >= 8) {
				cursorX = 7;
			} else if (cursorY == 4 && cursorX >= 3) {
				cursorX = 2;
			} else if (cursorX == 0 || (cursorX <= 2 && cursorY == 4)) {
				cursorX = 11;  // Wrap around
			} else {
				cursorX--;
			}
		}
		if (Pokitto::Buttons::repeat(BTN_RIGHT, 5)) {
			if (cursorY == 4 && cursorX < 3) {
				cursorX = 3;
			} else if (cursorY == 4 && cursorX < 8) {
				cursorX = 8;
			}
            else if (cursorY == 4 && cursorX < 10) {
				cursorX = 10;

			} else if (cursorX == 11 || (cursorX >=10 && (cursorY == 0 || cursorY == 4))) {
				cursorX = 0;  // Wrap around
			} else {
				cursorX++;
			}
		}
		// cursorY movement
		cursorY += Pokitto::Buttons::repeat(BTN_DOWN, 5) - Pokitto::Buttons::repeat(BTN_UP, 5);
		if (cursorY > 4) cursorY = 0;
		else if (cursorY < 0) cursorY = 4;

		keyboardDrawCursor(cursorX, cursorY, curLayout);


		// check for other button presses
		bool backspace = Pokitto::Buttons::pressed(BTN_B);
		bool switchLayout = Pokitto::Buttons::pressed(BTN_C);
		if (Pokitto::Buttons::pressed(BTN_A)) {
			char c = keyboardGetChar(cursorX, cursorY, keyboardLayout[curLayout]);
			if (!c) {
				// other handling
				if (cursorY == 0) {
					backspace = true;
				} else if (cursorY == 4 && cursorX < 3) {
					switchLayout = true;
                } else if (cursorY == 4 && cursorX < 10) {
                    text[0]=0;
					break;
				} else {
					// we are done!
					break;
				}
			} else if (activeChar < length) {
				text[activeChar++] = c;
				text[activeChar] = '\0';

			}
		}
		if (backspace) {
			if (activeChar > 0) {
				text[--activeChar] = '\0';
			}
		}
		if (switchLayout) {
			if (++curLayout >= keyboardLayoutPages) {
				curLayout = 0;
			}
			keyboardDrawLayout(keyboardLayout[curLayout]);

		}

		// render drawing text
        Pokitto::Display::setColor(WHITE);
		Pokitto::Display::fillRoundRect(5, keyboardYOffset+12, Pokitto::Display::width-10, 22, 2);

		Pokitto::Display::setColor(activeChar?BLACK:DARK_BLUE);
		Pokitto::Display::setCursor(7, keyboardYOffset+14);
		Pokitto::Display::print(text);

        Pokitto::Display::setColor(LIGHT_BLUE);
		Pokitto::Display::drawFastHLine(7+strlen(text)*12 , keyboardYOffset+30, 10);
	};
	Pokitto::Display::fontSize=1;
}



void drawPopup(std::string title, std::string text,unsigned char color)
{
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRoundRect(10,38,Pokitto::Display::width-20,100,5);
    Pokitto::Display::setColor(color);
    Pokitto::Display::drawRoundRect(10,38,Pokitto::Display::width-20,100,5);

    Pokitto::Display::drawFastHLine(15,60,Pokitto::Display::width-30);

    Pokitto::Display::setColor(color);
    Pokitto::Display::setCursor(110-(title.length()*3),45);
    Pokitto::Display::print(title.c_str());


    Pokitto::Display::setCursor(0,70);
    Pokitto::Display::print(fmt(4,32, text).c_str());

}

bool YesNo(std::string title, std::string text)
{
    drawPopup(title,text,BLACK);

    Pokitto::Display::setColor(GREEN);
    Pokitto::Display::fillRoundRect(55,115,50,15,3);
    Pokitto::Display::setColor(RED);
    Pokitto::Display::fillRoundRect(115,115,50,15,3);

    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(62,115+4);
    Pokitto::Display::print("A: Yes");
    Pokitto::Display::setCursor(125,115+4);
    Pokitto::Display::print("B: No");


    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        if(Pokitto::Buttons::pressed(BTN_A))
            return true;
        if(Pokitto::Buttons::pressed(BTN_B))
            return false;
    }
}

bool Error(std::string title, std::string text, size_t code)
{
    char code_string [10];
    sprintf(code_string, " %d", code);
    drawPopup(title + code_string,text,ORANGE);

    Pokitto::Display::setColor(RED);
    Pokitto::Display::fillRoundRect(75,115,70,15,3);

    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(86,115+4);
    Pokitto::Display::print("A: Close");


    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        if(Pokitto::Buttons::pressed(BTN_A))
            return true;
        if(Pokitto::Buttons::pressed(BTN_B))
            return false;
    }
}

std::string fmt(size_t margin, size_t width, std::string text)
{
    std::string result;
    while (!text.empty())   // while more text to move into result
    {
        result += std::string(margin, ' ');  // add margin for this line

        if (width >= text.size())  // rest of text can fit... nice and easy
            return (result += text) += '\n';

        size_t n = width - 1;  // start by assuming we can fit n characters
        while (n > width / 2 && isalnum(text[n]) && isalnum(text[n - 1]))
            --n; // between characters; reduce n until word breaks or 1/2 width left

        // move n characters from text to result...
        (result += text.substr(0, n)) += '\n';
        text.erase(0, n);
    }
    return result;
}

bool PopInfo(std::string path)
{
    Pop::open(path);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRoundRect(10,18,Pokitto::Display::width-20,140,5);
    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::drawRoundRect(10,18,Pokitto::Display::width-20,140,5);

    Pokitto::Display::drawFastHLine(15,68,Pokitto::Display::width-30);

    Pokitto::Display::setColor(DARK_BLUE);
    Pokitto::Display::setCursor(15+36+5,25);
    Pokitto::Display::print(Pop::getLongName().c_str());
    Pokitto::Display::setColor(PURPLE);
    Pokitto::Display::setCursor(15+36+5,25+10);
    Pokitto::Display::print("by ");
    Pokitto::Display::print(Pop::getAuthor().c_str());
    Pokitto::Display::setCursor(15+36+5,25+20);
    Pokitto::Display::printf("Size: %d.%01d KB",Pop::popFile->flen()/1024,(Pop::popFile->flen()%1024)/102);


    if(!Pop::drawIcon36(15,25))
        Pop::drawIcon24(15,25);

    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::setCursor(0,75);
    Pokitto::Display::print(fmt(4,32, Pop::getDescription()).c_str());

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRoundRect(75-53,137,100,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(86-53,137+4);
    Pokitto::Display::print("A: Screenshots");

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRoundRect(175-48,137,70,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(186-48,137+4);
    Pokitto::Display::print("B: Close");

    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        if(Pokitto::Buttons::pressed(BTN_A))
            return true;
        if(Pokitto::Buttons::pressed(BTN_B))
        {
            Pop::close();
            return false;
        }

    }
}

void fileInfo(std::string path)
{
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRoundRect(10,18,Pokitto::Display::width-20,140,5);
    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::drawRoundRect(10,18,Pokitto::Display::width-20,140,5);

    Pokitto::Display::drawFastHLine(15,68,Pokitto::Display::width-30);

    Pokitto::Display::setColor(DARK_BLUE);
    Pokitto::Display::setCursor(0,25);
    Pokitto::Display::print(fmt(4,30, path).c_str());

    static FILINFO info;
    f_stat(path.c_str(), &info);

    Pokitto::Display::setColor(PURPLE);
    char si[30];
    sprintf(si, "Size: %u.%01u KB",info.fsize/1024,(info.fsize%1024)/10);
    Pokitto::Display::print(fmt(4,30, std::string(si)).c_str());

    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::setCursor(18,75);
    Pokitto::Display::printf("Short Name: %s", info.fname);

    Pokitto::Display::setCursor(18,75+13);
    Pokitto::Display::printf("Timestamp: %u/%02u/%02u, %02u:%02u",(info.fdate >> 9) + 1980, info.fdate >> 5 & 15, info.fdate & 31,info.ftime >> 11, info.ftime >> 5 & 63);
    Pokitto::Display::setCursor(18,75+26);
    Pokitto::Display::printf("Attributes: %c%c%c%c%c",
               (info.fattrib & AM_DIR) ? 'D' : '-',
               (info.fattrib & AM_RDO) ? 'R' : '-',
               (info.fattrib & AM_HID) ? 'H' : '-',
               (info.fattrib & AM_SYS) ? 'S' : '-',
               (info.fattrib & AM_ARC) ? 'A' : '-');

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRoundRect(75,137,70,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(86,137+4);
    Pokitto::Display::print("A: Close");


    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        if(Pokitto::Buttons::pressed(BTN_A))
            return;
        if(Pokitto::Buttons::pressed(BTN_B))
            return;
    }
}

size_t Menu(std::string title, std::string entries[], const unsigned char *icons [], size_t NumOfEntries)
{
    size_t selected=0;
    size_t menuYOffset=(176-10*NumOfEntries)/2;

    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        Pokitto::Display::setColor(WHITE);
        Pokitto::Display::fillRoundRect(10,menuYOffset-10,Pokitto::Display::width-20,20+10*NumOfEntries,5);
        Pokitto::Display::setColor(BLACK);
        Pokitto::Display::drawRoundRect(10,menuYOffset-10,Pokitto::Display::width-20,20+10*NumOfEntries,5);

        Pokitto::Display::setColor(BLACK);
        Pokitto::Display::setCursor(20,menuYOffset-7);
        Pokitto::Display::print(title.c_str());
        Pokitto::Display::drawFastHLine(15,menuYOffset+3,Pokitto::Display::width-30);

        for(auto i=0;i<NumOfEntries;i++)
        {
            if(i==selected)
            {
                Pokitto::Display::setColor(LIGHT_BLUE);
                Pokitto::Display::fillRect(11,5+10*i+menuYOffset,Pokitto::Display::width-22,10);
                Pokitto::Display::setColor(WHITE);
            }
            else
                Pokitto::Display::setColor(BLACK);

            Pokitto::Display::setCursor(35,7+10*i+menuYOffset);
            Pokitto::Display::print(entries[i].c_str());
            if(icons !=0 )
                Pokitto::Display::drawBitmap(20,7+10*i+menuYOffset,icons[i]);
        }

        if(Pokitto::Buttons::repeat(BTN_UP,3))
        {
            if(selected > 0)
                selected--;
        }

        if(Pokitto::Buttons::repeat(BTN_DOWN,3))
        {
            if(selected < NumOfEntries-1)
                selected++;
        }

        if(Pokitto::Buttons::pressed(BTN_A))
            return selected;
        if(Pokitto::Buttons::pressed(BTN_B))
            return -1;
    }
}

void progressBar(std::string title, size_t value)
{
    drawPopup(title,"",RED);

    if(value > 100)
        value=100;

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRoundRect(15,90,190,25,3);
    Pokitto::Display::setColor(GREEN);
    auto pr=(190*value)/100;

    if(pr > 3)
        Pokitto::Display::fillRoundRect(15,90,pr,25,3);

    Pokitto::Display::update();

}

void InfoBox(std::string title, std::string text)
{
    drawPopup(title,text,DARK_GREEN);

    Pokitto::Display::setColor(DARK_GREEN);
    Pokitto::Display::fillRoundRect(75,115,70,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(86,115+4);
    Pokitto::Display::print("A: Close");
    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        if(Pokitto::Buttons::pressed(BTN_A))
            return;
        if(Pokitto::Buttons::pressed(BTN_B))
            return;
    }
}

void Loading(std::string text)
{
    drawPopup("Please Wait", text,BLUE);
    Pokitto::Display::update();
}

size_t timePicker(std::string title, bool format24,size_t value)
{
    unsigned int selected=0;
    unsigned short h = value >> 16;
    unsigned short m = value &0xFFFF;
    while(true)
    {
        if(!Pokitto::Core::update())
            continue;

        drawPopup(title,"", DARK_GRAY);


        Pokitto::Display::fontSize=2;
        if(selected==0)
            Pokitto::Display::setColor(LIGHT_BLUE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::fillTriangle(85,75,95,75,90,70);
        Pokitto::Display::fillTriangle(85,100,95,100,90,105);

        Pokitto::Display::setCursor(80-2,80);
        Pokitto::Display::printf("%02u", format24?h:(h>12?h-12:(h==0?12:h)));

        Pokitto::Display::setColor(BLACK);
        Pokitto::Display::setCursor(105,80);
        Pokitto::Display::print(':');

        if(selected==1)
            Pokitto::Display::setColor(LIGHT_BLUE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::fillTriangle(125,75,135,75,130,70);
        Pokitto::Display::fillTriangle(125,100,135,100,130,105);

        Pokitto::Display::setCursor(120,80);
        Pokitto::Display::printf("%02u",m);


        if(!format24)
        {
            Pokitto::Display::setColor(BLACK);
            Pokitto::Display::setCursor(150,80);
            if(h > 11)
                Pokitto::Display::print("PM");
            else
                Pokitto::Display::print("AM");
        }


        Pokitto::Display::fontSize=1;
        Pokitto::Display::setColor(GREEN);
        Pokitto::Display::fillRoundRect(45,115,50,15,3);
        Pokitto::Display::setColor(RED);
        Pokitto::Display::fillRoundRect(105,115,70,15,3);

        Pokitto::Display::setColor(WHITE);
        Pokitto::Display::setCursor(52,115+4);
        Pokitto::Display::print("A: Ok");
        Pokitto::Display::setCursor(115,115+4);
        Pokitto::Display::print("B: Cancel");


        if(Pokitto::Buttons::repeat(BTN_UP,4))
        {
            if(selected==0)
                h++;
            if(selected==1)
                m++;

            if(h==24)
                h=0;
            if(m==60)
                m=0;

        }else if(Pokitto::Buttons::repeat(BTN_DOWN,4))
        {
             if(selected==0)
                h--;
            if(selected==1)
                m--;

            if(h==0xFFFF)
                h=23;
            if(m==0xFFFF)
                m=59;
        }

        if(Pokitto::Buttons::pressed(BTN_RIGHT))
        {
            if(selected < 1)
                selected++;

        }else if(Pokitto::Buttons::pressed(BTN_LEFT))
        {
             if(selected > 0)
                selected--;
        }

        if(Pokitto::Buttons::pressed(BTN_A))
            return (h<<16)|m;
        if(Pokitto::Buttons::pressed(BTN_B))
            return 0xFFFFFFFF;
    }
}

size_t datePicker(std::string title, unsigned char dateFormat, size_t value)
{
    unsigned int selected=0;
    unsigned short y = value >> 16;
    unsigned short m = (value>>8) & 0xFF;
    unsigned short d = value & 0xFF;
    while(true)
    {
        if(!Pokitto::Core::update())
            continue;

        drawPopup(title,"", DARK_GRAY);


        Pokitto::Display::fontSize=2;
        if(selected==0)
            Pokitto::Display::setColor(LIGHT_BLUE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::fillTriangle(60,75,70,75,65,70);
        Pokitto::Display::fillTriangle(60,100,70,100,65,105);

        Pokitto::Display::setCursor(55,80);
        Pokitto::Display::printf("%02u", dateFormat==0?d:m+1);


        if(selected==1)
            Pokitto::Display::setColor(LIGHT_BLUE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::fillTriangle(90,75,100,75,95,70);
        Pokitto::Display::fillTriangle(90,100,100,100,95,105);

        Pokitto::Display::setCursor(85,80);
        Pokitto::Display::printf("%02u",dateFormat==0?m+1:d);

        if(selected==2)
            Pokitto::Display::setColor(LIGHT_BLUE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::fillTriangle(130,75,140,75,135,70);
        Pokitto::Display::fillTriangle(130,100,140,100,135,105);

        Pokitto::Display::setCursor(115,80);
        Pokitto::Display::printf("%04u",y+1900);


        Pokitto::Display::fontSize=1;
        Pokitto::Display::setColor(GREEN);
        Pokitto::Display::fillRoundRect(45,115,50,15,3);
        Pokitto::Display::setColor(RED);
        Pokitto::Display::fillRoundRect(105,115,70,15,3);

        Pokitto::Display::setColor(WHITE);
        Pokitto::Display::setCursor(52,115+4);
        Pokitto::Display::print("A: Ok");
        Pokitto::Display::setCursor(115,115+4);
        Pokitto::Display::print("B: Cancel");


        if(Pokitto::Buttons::repeat(BTN_UP,4))
        {
            if(selected==0)
                dateFormat==0?d++:m++;
            if(selected==1)
                dateFormat==0?m++:d++;
            if(selected==2)
                y++;

            if(d==32)
                d=1;
            if(m==12)
                m=0;

        }else if(Pokitto::Buttons::repeat(BTN_DOWN,4))
        {
            if(selected==0)
                dateFormat==0?d--:m--;
            if(selected==1)
                dateFormat==0?m--:d--;
             if(selected==2)
                y--;

            if(d==0)
                d=31;
            if(m==0xFFFF)
                m=11;
            if(y==0xFFFF)
                y=9999;
        }

        if(Pokitto::Buttons::pressed(BTN_RIGHT))
        {
            if(selected < 2)
                selected++;

        }else if(Pokitto::Buttons::pressed(BTN_LEFT))
        {
             if(selected > 0)
                selected--;
        }

        if(Pokitto::Buttons::pressed(BTN_A))
            return (y<<16) | (m<<8) | d;
        if(Pokitto::Buttons::pressed(BTN_B))
            return 0xFFFFFFFF;
    }
}

unsigned char volumeSlider(std::string title, unsigned char value, unsigned char maxValue)
{
    unsigned short V=value;
    drawPopup(title,"",DARK_GREEN);

    Pokitto::Display::setColor(GREEN);
    Pokitto::Display::fillRoundRect(45,115,50,15,3);
    Pokitto::Display::setColor(RED);
    Pokitto::Display::fillRoundRect(105,115,70,15,3);

    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(52,115+4);
    Pokitto::Display::print("A: Ok");
    Pokitto::Display::setCursor(115,115+4);
    Pokitto::Display::print("B: Cancel");

    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::fillTriangle(25,85,25,95,20,90);
    Pokitto::Display::fillTriangle(195,85,195,95,200,90);

    while(true)
    {
        if(!Pokitto::Core::update())
            continue;

        Pokitto::Display::setColor(LIGHT_GRAY);
        Pokitto::Display::fillRoundRect(35,80,150,20,3);
        Pokitto::Display::setColor(GREEN);
        auto pr=(150*V)/maxValue;
        if(pr > 1)
            Pokitto::Display::fillRoundRect(35,80,pr,20,3);

        if(Pokitto::Buttons::repeat(BTN_LEFT,5) && V > 0)
            V--;

        if(Pokitto::Buttons::repeat(BTN_RIGHT,5) && V < maxValue)
            V++;

        if(Pokitto::Buttons::pressed(BTN_A))
            return V;
        if(Pokitto::Buttons::pressed(BTN_B))
            return 0xFF;
    }
}

bool LoaderInfo(std::string path)
{
    FileHandle *loaderfile=sdFs->open(path.c_str(),O_RDONLY);
    if(!loaderfile)
        return false;

    size_t fsize=loaderfile->flen();
    loaderfile->lseek(fsize-8,SEEK_SET);
    size_t version= 0;
    loaderfile->read(&version,4);
    loaderfile->close();


    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRoundRect(10,38,Pokitto::Display::width-20,100,5);
    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::drawRoundRect(10,38,Pokitto::Display::width-20,100,5);

    Pokitto::Display::drawBitmap(15,25+20, Icons::Files::Der);
    Pokitto::Display::drawFastHLine(15,68+20, Pokitto::Display::width-30);

    Pokitto::Display::setColor(DARK_BLUE);
    Pokitto::Display::setCursor(15+36+5,25+20);
    Pokitto::Display::print(path.c_str());
    Pokitto::Display::setColor(PURPLE);
    Pokitto::Display::setCursor(15+36+5,25+10+20);
    Pokitto::Display::print("Version ");
    Pokitto::Display::print(static_cast<int>(version));
    Pokitto::Display::setCursor(15+36+5,25+20+20);
    Pokitto::Display::printf("Size: %d.%01d KB",fsize/1024,(fsize%1024)/102);

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRoundRect(75-53,137-20,85,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(86-53,137+4-20);
    Pokitto::Display::print("A: Install");

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRoundRect(175-48,137-20,70,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(186-48,137+4-20);
    Pokitto::Display::print("B: Close");

    while(true)
    {
        if(!Pokitto::Core::update())
            continue;
        if(Pokitto::Buttons::pressed(BTN_A))
            return true;
        if(Pokitto::Buttons::pressed(BTN_B))
        {
            Pop::close();
            return false;
        }

    }
}

}; //namespace Popup
