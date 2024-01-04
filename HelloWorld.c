void main()
{
	int line = 5;
	char str[11] = "HelloWorld";
	
	char *video = (char*)(0xB8000 + 160 * line);

	for (int i = 0; str[i] != 0; i++) {
		*video++ = str[i];
		*video++ = 0x03;
	}
	return;
}
