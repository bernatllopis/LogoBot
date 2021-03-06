#include "LogobotText.h"

// local copy of critical command defines - for space optimisation
#define LOGO_CMD_TO			4
#define LOGO_CMD_BZ			12
#define LOGO_CMD_PU			13
#define LOGO_CMD_PD			14
#define LOGO_CMD_NL			21

namespace LogobotText
{
	// Namespace containing private functions
	namespace
	{
		float fontSize;  // =em, equal to line spacing (between baselines), text sizes derived from this
		float capHeight;
		float letterSpacing;
		float w;

		CommandQueue * _cmdQ;

		// forward declarations
		void writeG(float x, float y);
		void writeO(float x, float y);
		void writeForwardSlash(float x, float y);



		void pushPU() {
			_cmdQ->enqueue("", LOGO_CMD_PU);
		}

		void pushPD() {
			_cmdQ->enqueue("", LOGO_CMD_PD);
		}

		void pushTo(float x, float y)
		{
			String s = String(x);
			s += " ";
			s += y;
			_cmdQ->enqueue(s, LOGO_CMD_TO);
		}

		/*
			Alphabet
		*/

		void writeZero(float x, float y) {
			writeO(x,y);
			writeForwardSlash(x,y);
		}

		void writeOne(float x, float y) {
			pushTo(x + w/3, y + 0.7 * capHeight);
			pushPD();
			pushTo(x + 2*w/3, y + capHeight);
			pushTo(x + 2*w/3, y);
		}

		void writeTwo(float x, float y) {
			pushTo(x, y + 0.7 * capHeight);
			pushPD();
			pushTo(x + w/2, y + capHeight);
			pushTo(x + w, y + 0.7* capHeight);
			pushTo(x, y);
			pushTo(x + w, y);
		}

		void writeThree(float x, float y) {
			pushPD();
			pushTo(x + 2 * w / 3, y);
			pushTo(x + w, y + capHeight / 4);
			pushTo(x + 2 * w / 3, y + capHeight / 2);
			pushTo(x + w, y + 3 * capHeight / 4);
			pushTo(x + 2 * w / 3, y + capHeight);
			pushTo(x, y + capHeight);
		}

		void writeFour(float x, float y) {
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x, y + capHeight / 2);
			pushTo(x + w, y + capHeight / 2);
			pushTo(x + w, y + capHeight);
			pushTo(x + w, y);
		}

		void writeSix(float x, float y) {
			writeG(x,y);
			pushTo(x, y + capHeight/3);
		}

		void writeSeven(float x, float y) {
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x + w, y + capHeight);
			pushTo(x,y);
		}

		void writeEight(float x, float y) {
			writeO(x,y);
			pushTo(x, y + capHeight/2);
			pushTo(x + w, y + capHeight/2);
		}

		void writeNine(float x, float y) {
			pushPD();
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight);
			pushTo(x, y + capHeight);
			pushTo(x, y + capHeight/2);
			pushTo(x + w, y + capHeight/2);
		}

		void writeA(float x, float y)
		{
			pushPD();
			pushTo(x + w/2, y + capHeight);
			pushTo(x + w, y);
			pushPU();
			pushTo(x + w / 4, y + capHeight / 2);
			pushPD();
			pushTo(x + 3 * w / 4, y + capHeight / 2 );
		}

		void writeB(float x, float y)
		{
			writeThree(x,y);
			pushTo(x, y);
		}

		void writeC(float x, float y)
		{
			pushTo(x + w, y + capHeight);
			pushPD();
			pushTo(x, y + capHeight);
			pushTo(x, y);
			pushTo(x + w, y);
		}

		void writeD(float x, float y)
		{
			pushPD();
			pushTo(x + 3 * w / 4, y);
			pushTo(x + w, y + capHeight / 4);
			pushTo(x + w, y + 3 * capHeight / 4);
			pushTo(x + 3 * w / 4, y + capHeight);
			pushTo(x, y + capHeight);
			pushTo(x, y);
		}

		void writeE(float x, float y)
		{
			writeC(x, y);
			pushPU();
			pushTo(x, y + capHeight / 2);
			pushPD();
			pushTo(x + w, y + capHeight /2);
		}

		void writeF(float x, float y)
		{
			pushPD();
			pushTo(x, y + capHeight);
			pushTo(x + w, y + capHeight);
			pushPU();
			pushTo(x + w, y + capHeight / 2);
			pushPD();
			pushTo(x, y + capHeight / 2);
		}

		void writeG(float x, float y)
		{
			pushTo(x + w, y + capHeight);
			pushPD();
			pushTo(x, y + capHeight);
			pushTo(x, y);
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight/3);
			pushTo(x + w/3, y + capHeight/3);
		}

		void writeH(float x, float y)
		{
			pushPD();
			pushTo(x, y + capHeight/2);
			writeFour(x,y);
		}

		void writeI(float x, float y)
		{
			pushTo(x + w / 2, y);
			pushPD();
			pushTo(x + w / 2, y + capHeight);
		}

		void writeJ(float x, float y)
		{
			pushTo(x, y + capHeight / 4);
			pushPD();
			pushTo(x, y);
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight);
		}

		void writeK(float x, float y)
		{
			pushPD();
			pushTo(x, y + capHeight);
			pushPU();
			pushTo(x + w, y + capHeight);
			pushPD();
			pushTo(x, y + capHeight / 2);
			pushTo(x + w, y);
		}

		void writeL(float x, float y)
		{
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x,y);
			pushTo(x + w, y);
		}

		void writeM(float x, float y)
		{
			pushPD();
			pushTo(x, y + capHeight);
			pushTo(x + w / 2, y + capHeight / 2);
			pushTo(x + w, y + capHeight);
			pushTo(x + w, y);
		}

		void writeN(float x, float y)
		{
			pushPD();
			pushTo(x, y + capHeight);
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight);

		}

		void writeO(float x, float y)
		{
			pushPD();
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight);
			pushTo(x, y + capHeight);
			pushTo(x, y);
		}

		void writeP(float x, float y)
		{
			pushPD();
			pushTo(x, y + capHeight);
			pushTo(x + w, y + capHeight);
			pushTo(x + w, y + capHeight / 2);
			pushTo(x, y + capHeight / 2);
		}

		void writeQ(float x, float y)
		{
			pushPD();
			pushTo(x + w / 2, y);
			pushTo(x + w, y + capHeight / 2);
			pushTo(x + w, y + capHeight);
			pushTo(x, y + capHeight);
			pushTo(x, y);
			pushPU();
			pushTo(x + w / 2, y + capHeight / 2);
			pushPD();
			pushTo(x + w, y);
		}

		void writeR(float x, float y)
		{
			writeP(x, y);
			pushTo(x + w, y);
		}

		void writeSFive(float x, float y, float skew)
		{
			pushPD();
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight * (0.5 - skew));
			pushTo(x, y + capHeight * (0.5 + skew));
			pushTo(x, y + capHeight);
			pushTo(x + w, y + capHeight);
		}

		void writeT(float x, float y)
		{
			pushTo(x + w/2, y);
			pushPD();
			pushTo(x + w/2, y + capHeight);
			pushTo(x, y + capHeight);
			pushTo(x + w, y + capHeight);
		}

		void writeU(float x, float y)
		{
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x, y);
			pushTo(x + w, y);
			pushTo(x + w, y + capHeight);
		}

		void writeV(float x, float y)
		{
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x + w / 2, y);
			pushTo(x + w, y + capHeight);
		}

		void writeW(float x, float y)
		{
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x + w / 4, y);
			pushTo(x + w / 2, y + capHeight / 2);
			pushTo(x + 3 * w / 4, y);
			pushTo(x + w, y + capHeight);
		}

		void writeX(float x, float y)
		{
			pushPD();
			pushTo(x + w, y + capHeight);
			pushPU();
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x + w, y);
		}

		void writeY(float x, float y)
		{
			pushPD();
			pushTo(x + w, y + capHeight);
			pushPU();
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x + w / 2, y + capHeight / 2);
		}

		void writeZ(float x, float y)
		{
			pushTo(x, y + capHeight);
			pushPD();
			pushTo(x + w, y + capHeight);
			pushTo(x, y);
			pushTo(x + w, y);
		}

		void writeColon(float x, float y)
		{
			pushTo(x + w / 3, y + capHeight / 3);
			pushPD();
			pushTo(x + 2 * w / 3, y + capHeight / 3);
			pushPU();
			pushTo(x + 2 * w / 3, y + 2 * capHeight / 3);
			pushPD();
			pushTo(x + w / 3, y + 2 * capHeight / 3);
		}

		void writeCloseBracket(float x, float y)
		{
			pushTo(x + w / 3, y);
			pushPD();
			pushTo(x + 2 * w / 3, y + capHeight / 4);
			pushTo(x + 2 * w / 3, y + 3 * capHeight / 4);
			pushTo(x + w / 3, y + capHeight);
		}

		void writeHash(float x, float y)
		{
			pushTo(x, y + capHeight / 3);
			pushPD();
			pushTo(x + w, y + capHeight / 3);
			pushPU();
			pushTo(x + w, y + 2 * capHeight / 3);
			pushPD();
			pushTo(x, y + 2 * capHeight / 3);
			pushPU();

			pushTo(x + w / 3, y + capHeight);
			pushPD();
			pushTo(x + w / 3, y);
			pushPU();
			pushTo(x + 2 * w / 3, y);
			pushPD();
			pushTo(x + 2 * w / 3, y + capHeight);
		}

		void writeAt(float x, float y)
		{
			pushTo(x + 2 * w / 3, y + capHeight / 3);
			pushPD();
			pushTo(x + 2 * w / 3, y + 2 * capHeight / 3);
			pushTo(x + w / 3, y + capHeight / 2);
			pushTo(x + 2 * w / 3, y + capHeight / 3);
			pushTo(x + w, y + capHeight / 2);
			pushTo(x + 2 * w / 3, y + capHeight);
			pushTo(x, y + capHeight / 2);
			pushTo(x + w / 2, y);
			pushTo(x + w, y + capHeight / 3);
		}

		void writeForwardSlash(float x, float y) {
			pushPD();
			pushTo(x + w, y + capHeight);
		}
	}
	// End private namespace functions

	// Logobot text public functions
	void begin(CommandQueue& cmdQ)
	{
		_cmdQ = &cmdQ;
		setFontSize(20);
	}

	void setFontSize(float size)
	{
		fontSize = size;
		capHeight = fontSize * 0.7;
		letterSpacing = fontSize * 0.1;
		w = fontSize * 0.5;
	}

	float getFontSize() {
		return fontSize;
	}

	void writeChar(char c, float x, float y)
	{
		switch (c) {
			case '\n':
				_cmdQ->enqueue("", LOGO_CMD_NL);
				break;
			case '0':
				writeZero(x,y);
				break;
			case '1':
				writeOne(x,y);
				break;
			case '2':
				writeTwo(x,y);
				break;
			case '3':
				writeThree(x,y);
				break;
			case '4':
				writeFour(x,y);
				break;
			case '5':
				writeSFive(x,y,0);
				break;
			case '6':
				writeSix(x,y);
				break;
			case '7':
				writeSeven(x,y);
				break;
			case '8':
				writeEight(x,y);
				break;
			case '9':
				writeNine(x,y);
				break;
			case 'A':
				writeA(x, y);
				break;
			case 'B':
				writeB(x, y);
				break;
			case 'C':
				writeC(x, y);
				break;
			case 'D':
				writeD(x, y);
				break;
			case 'E':
				writeE(x, y);
				break;
			case 'F':
				writeF(x, y);
				break;
			case 'G':
				writeG(x, y);
				break;
			case 'H':
				writeH(x, y);
				break;
			case 'I':
				writeI(x, y);
				break;
			case 'J':
				writeJ(x, y);
				break;
			case 'K':
				writeK(x, y);
				break;
			case 'L':
				writeL(x, y);
				break;
			case 'M':
				writeM(x, y);
				break;
			case 'N':
				writeN(x, y);
				break;
			case 'O':
				writeO(x, y);
				break;
			case 'P':
				writeP(x, y);
				break;
			case 'Q':
				writeQ(x, y);
				break;
			case 'R':
				writeR(x, y);
				break;
			case 'S':
				writeSFive(x, y, 0.1);
				break;
			case 'T':
				writeT(x, y);
				break;
			case 'U':
				writeU(x, y);
				break;
			case 'V':
				writeV(x, y);
				break;
			case 'W':
				writeW(x, y);
				break;
			case 'X':
				writeX(x, y);
				break;
			case 'Y':
				writeY(x, y);
				break;
			case 'Z':
				writeZ(x, y);
				break;
			case ' ':
				// nothing to do, just move to next letter
				break;
			case ':':
				writeColon(x, y);
				break;
			case ')':
				writeCloseBracket(x, y);
				break;
			case '#':
				writeHash(x, y);
				break;
			case '@':
				writeAt(x, y);
				break;
			case '/':
				writeForwardSlash(x,y);
				break;

			default:
				_cmdQ->enqueue("500", LOGO_CMD_BZ);
				return;
		}

		pushPU();
		pushTo(x + w + letterSpacing, y);
	}
}
