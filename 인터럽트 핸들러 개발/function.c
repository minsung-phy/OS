void kprintf(char* str, int line, int col) // str 글자를 line 행 col 열에 출력하는 함수
{
	char *video = (char*)(0xB8000 + 2 * (line * 80 + col));

	for (int i = 0; str[i] != 0; i++) {
		*video++ = str[i];
		*video++ = 0x03;
	}

	return;
}
