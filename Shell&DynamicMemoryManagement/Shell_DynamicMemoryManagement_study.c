// 2. 쉘 개발
// * 키 버퍼
// 셸을 잠재적으로 하위 명령과 인수가 있는 복잡한 명령을 지원해야 합니다.
// 즉, 단일 키 명령만으로는 큰 도움이 되지 않고 사용자가 여러 문자로 구성된 명령을 입력하는 것이 좋습니다.
// 하지만 명령을 입력할 때 이를 저장할 공간이 필요합니다. 이때 키 버퍼가 필요합니다.

// 키 버퍼를 문자 배열로 구현할 수 있습니다. 0바이트로 초기화되고 키 누름은 인덱스 0부터 위쪽으로
// 기록됩니다. 이 데이터 구조를 조금 더 자세히 살펴보면 문자열을 인코딩하는 방식과 같다는 것을
// 알 수 있습니다. 0바이트로 끝나는 일련의 문자들입니다.

// 키 버퍼를 효율적으로 사용하려면 두 개의 문자열 유틸리티 함수가 더 필요합니다.
// 문자열의 길이를 계산하는 함수와 주어진 문자열에 문자를 추가하는 함수입니다.
// 후자의 함수는 전자를 사용하려고 합니다.

int string_length (char s[]) {
	int i = 0;
	while (s[i] != '\0') {
		++i;
	}
	return i;
}

void append(char s[], char n) {
	int len = string_length(s);
	s[len] = n;
	s[len+1] = '\0';
}

// 다음으로, 작주에 작성한 키보드 콜백 함수를 몇 가지 조정할 수 있습니다. 먼저, 방대한 스위치 문을
// 없애고 스캔 코드에 기반한 배열 조회로 대체하겠습니다.
// 둘째, 모든 키 업 및 영숫자가 아닌 스캔 코드를 무시합니다.
// 마지막으로 각 키를 키 버퍼에 기록하여 화면에 출력합니다.

#define SC_MAX 57

static char key_buffer[256];

const char scancode_to_char[] = {
	'?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
	'-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 'H',
	'J', 'K', 'L', ';', '\', '`', '?', '\\', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '
};

static void keyboard_callback(registers_t *regs) {
	uint8_t scancode = port_byte_in(0x60);
	
	if (scancode > SC_MAX)
		return;
	
	char letter = scancode_to_char[(int) scancode];
	append(key_buffer, letter);
	char str[2] = {letter, '\0'};
	print_string(str);
}

// 이 방법은 작동하지만 두 가지 문제가 있습니다. 
// 첫째, 추가하기 전에 키 버퍼의 경계를 확인하지 않아 버퍼 오버플로우의 위험이 있습니다.
// 둘째, 명령을 입력할 때 실수할 여지를 남기지 않습니다.

// * 백스페이스
// 사용자는 백스페이스를 눌러 오타를 수정할 수 있어야 하며, 버퍼와 화면에서 마지막 문자를 효과적으로
// 삭제할 수 있어야 합니다.

// 버퍼 수정을 구현하는 방법은 추가 기능을 역으로 사용하면 됩니다.
// 버퍼의 마지막 0이 아닌 바이트를 0으로 설정하기만 하면 됩니다.
// 이 방법은 버퍼에서 요소를 성공적으로 제거하면 참을 변환하고 그렇지 않으면 거짓을 반환합니다.
// 참고: #include <stdbool.h>를 사용하여 bool에 대한 타입 정의를 가져와야 합니다.

bool backspace(char buffer[]) {
	int len = string_length(buffer);
	if (len > 0) {
		buffer[len - 1] = '\0';
		return true;
	} else {
		return false;
	}
}

// 화면에 백스페이스 문자를 인쇄하는 것은 현재 커서 위치 바로 앞의 위치에 빈 문자를 인쇄하고 
// 커서를 뒤로 이동하는 방식으로 구현할 수 있습니다.
// 여기서는 VGA 드라이버의 get_cursor, set_cursor, set_char_at_video_memory 
// 함수를 활용하겠습니다.

void print_backspace() {
	int newCursor = get_cursor() - 2;
	set_char_at_video_memory(' ', newCursor);
	set_cursor(newCursor);
}

// 백스페이스 기능을 완성하기 위해 백스페이스 키 누름 전용 브랜치를 추가하여 
// 키보드 콜백 기능을 수정합니다.
// 백스페이스가 눌리면 먼저 키 버퍼에서 마지막 문자를 삭제하려고 시도합니다.
// 이 작업이 성공하면 화면에 백스페이스도 표시됩니다.
// 그렇지 않으면 사용자가 프롬포트에 의해 중단되지 않고 화면 끝까지 백스페이스를 누를 수 있기 때문에
// 이 검사를 수행하는 것이 중요합니다.

#define BACKSPACE 0x0E

static void keyboard_callback(registers_t *regs) {
	uint8_t scancode = port_byte_in(0x60);
	if (scancode > SC_MAX)
		return;
	if (scancoed == BACKSPACE) {
		if (backspace(key_buffer)) {
			print_backspace();
		}
	} else {
		char letter = scancode_to_char[(int) scancode];
		append(key_buffer, letter);
		char str[2] = {letter, '\0'};
		print_string(str);
	}
}

// 키 버퍼와 백스페이스 기능이 준비되었으므로 이제 마지막 단계인 
// 명령 구문 분석 및 실행으로 넘어갈 수 있습니다.

// * 명령 구문 분석 및 실행
// 사용자가 엔터 키를 누를 때마다 주어진 명령을 실행하고자 합니다.
// 이를 위해서는 일반적으로 먼저 명령을 구문 분석하여 여러 개의 하위 명령으로 분할하거나 인수를
// 구문 분석하거나 외부 기능을 호출하는 작업이 포함됩니다.
// 여기서는 간단하게 하기 위해 문자열이 알려진 명령어인지 확인하고 그렇지 않은 경우 오류를 
// 표시하는 아주 기본적인 구문분석만 구현하겠습니다.

// 먼저 두 문자열을 비교하는 함수를 작성해야 합니다. 이 함수는 두 문자열을 단계별로 살펴보면서
// 문자 값을 비교합니다. 다음은 코드입니다.

int compare_string(char s1[], char s2[]) {
	int i;
	for (i = 0; s1[i] == s2[i]; i++) {
		if (S1[i] == '\0')
			return 0;
	}
	return s1[i] - s2[i];
}

// 다음으로, 주어진 명령을 실행하는 함수 execute_command를 구현해야 합니다.
// 첫 번째 버전의 셸은 CPU를 중지하는 EXIT라는 명령어 하나만 인식합니다.
// 나중에 재부팅이나 파일 시스템과의 상호 작용과 같은 다은 명령을 구현할 수 있습니다.
// 명령을 알 수 없는 경우 오류 메시지를 출력합니다.
// 마지막으로 새 프롬포트를 인쇄합니다.

void execute_command(char *input) {
	if (compare_string(input, "EXIT") == 0) {
		print_string("Stopping the CPU. BYE!\n");
		asm volatile("hit");
	}
	print_string("Unknown command: ");
	print_string(input);
	print_string(\n> ");
}

// 마지막으로 키보드 콜백을 조정하여 커서를 다음 줄로 이동하고 실행 명령을 호출한 다음 엔터 키를 누르면 
// 키 버퍼를 재설정합니다.

#define ENTER 0x1C

static void keyboard_callback(registers_t *regs) {
	uint8_t scancode = port_byte_in(0x60);
	if (scancode > SC_MAX)
		return;
	
	if (scancode == BACKSPACE) {
		if (backspace(key_buffer)) {
			print_backspace();
		}
	}
	else if (scancode == ENTER) {
		print_nl();
		execute_command(key_buffer);
		key_buffer[0] = '\0';
	}
	else {
		char letter = scancode_to_char[(int) scancode];
		append(key_buffer, letter);
		char str[2] = {letter, '\0'};
		print_string(str);
	}
}

// 거의 다 끝났습니다. 이제 메인 커널 기능을 업데이트 해보겠습니다.

// * 업데이트 된 커널 기능
// 사실, 할 일이 많지 않습니다. 모든 초기화 작업이 완려된 후 화면을 지우고 초기 프롬포트를 표시하면
// 끝입니다. 나머지는 업데이트 된 키보드 핸들러가 알아서 처리합니다.
// 여기 코드와 데모가 있습니다.

void start_kernel() {
	clear_screen();
	print_string("Installing interrupt service routined (ISRS).\n")
	isr_install();
	
	print_string("Enabling external interrupts.\n");
	asm volatile("sti");
	
	print_string("Initializing keyboard (IRQ 1).\n");
	init_keyboard();
	
	clear_screen();
	print_string("> ");
}

//  3. 동적 메모리 관리 개발
// * 개요
// 지금까지는 문자열은 저장하는 등 메모리가 필요할 떄마다 다음과 같이 메모리를 할당했습니다
// char my_string[10];
// 이 문장은 문자를 저장하는 데 사용할 수 있는 10개의 연속 바이트를 메모리에 할당하도록
// C 컴파일러에 지시합니다.

// 하지만 컴파일 시점에 배열의 크기를 모른다면 어떻게 될까요?
// 사용자가 문자열의 길이를 지정하고 싶다고 가정해 봅시다.
// 물론 256 바이트와 같이 고정된 양의 메모리를 할당할 수 있습니다.
// 하지만 이 경우 너무 많은 양이 할당되어 메모리가 낭비될 가능성이 높습니다.
// 또 다른 결과는 정적으로 할당된 메모리가 충분하지 않아 
// 프로그램이 충돌하는 것입니다.

// 동적 메모리 관리로 이 문제를 해결할 수 있습니다. OS는 런타임에 결정되는 유연한 메모리 양을
// 할당할 수 있는 방법을 제공해야 합니다.
// 메모리 부족의 위험을 줄이려면 더 이상 사용하지 않는 메모리를 다시 사용할 수 있도록
// 하는 기능도 필요합니다.
// 이 블로그 게시물에서는 동적 메모리 관리를 위한 간단한 알고리즘을 설계하고
// 구현하고자 합니다.

// 이 글의 나머지 부분은 다음과 같은 구조로 구성되어 있습니다.
// 먼저 동적 메모리를 관리하는 데 사용할 데이터 구조와 할당 및 할당 해제 알고리즘을 
// 설계합니다. 그런 다음 이전 섹션의 이론을 기반으로 커널에 대한 동적 메모리 관리를 구현합니다.

// * 디자인
// - 데이터 구조
// 동적 메모리 관리를 구현하기 위해 프로그램의 일부에 개별 청크를 빌릴 수 있는 큰
// 메모리 영역을 정적으로 할당할 것 입니다.
// 빌린 메모리가 더 이상 필요하지 않으면 풀로 반환할 수 있습니다.
// 문제는 빌린 청크, 즉 동적으로 할당된 청크를 어떨게 추적할 수 있을까요?
// 최소한 요청된 크기의 사용 가능한 메모리를 찾을 수 있는 데이터 구조가 필요합니다.
// 또한 작은 메모리 조각이 많이 남는 것을 피하기 위해 가능한 한 가장 작은 사용 가능한 
// 영역을 선택하려고 합니다.

// 간단한 구현을 위해 이중으로 연결된 리스트를 사용하겠습니다.
// 각 요소는 해당 청크에 대한 정보, 특히 주소와 크기, 현재 할당 여부.
// 이전 및 다음 요소에 대한 포인터를 보유합니다.
// 전체 목록을 O(n)으로 반복하면 최적의, 즉 가능한 가장 작은 영역을 찾을 수 있습니다.
// 여기서 n은 목록의 요소 수입니다.
// 물론 힙과 같은 더 효율적인 대안도 있지만 구현하기가 더 복잡하므로 여기서는 리스트를
// 사용하겠습니다.

// 이제 리스트 어디에 저장해야할까요? 얼마나 많은 청크가 요청될지, 따라서 목록에
// 얼마나 많은 요소가 포함될지 알 수 없으므로 메모리를 정적으로 할당할 수 없습니다.
// 하지만 지금은 동적 메모리 할당을 구축 중이기 때문에 동적으로 할당할 수도 없습니다.

// 동적 할당을 위해 예약한 큰 메모리 영역에 목록 요소를 포함하면 이 문제를 극복할 수 있습니다.
// 요청되는 각 청크에 대해 해당 청크 바로 앞에 해당 목록 요소를 저장합니다.
// 다음 그림은 1024 바이트 동적 메모리 영역의 초기 상태를 보여줍니다.
// 여기에는 16바이트 목록 요소 (처음에 있는 작은 짙은 녹색 부분)가 하나 포함되어 있으며 나머지 
// 1008 바이트가 여유 공간임을 나타냅니다.

// 이제 할당 알고리즘에 대해 살펴보겠습니다.

// - 할당 알고리즘
// 크기 sm의 새 메모리 m이 요청되면 사용 가능한 모든 메모리를 살펴보고 최적의 청크를 찾습니다.
// 메모리 청크 L의 목록의 주어지면, 자유롭고, 충분히 크며, 더 작은 항목이 없는 
// 최적의 항목 o를 찾으려고 시도합니다.

// 최적의 세그먼트 o가 주어지면 요청된 메모리 양을 잘라내어 새 목록 항목을 포함하는
// 새 세그먼트 p를 생성하여 효과적으로 o를 so - sm - sl 크기 (여기서 sl은 목록 요소의 크기)
// 로 축소합니다. 새 세그먼트의 크기는 sp = sm + sl이 됩니다. 그런 다음 리스트 요소 바로
// 뒤에 할당된 메모리의 시작 부분에 대한 포인터를 반환합니다. 최적의 청크가 존재하지
// 않으면 할당에 실패한 것입니다.

// 다음 그림은 알고리즘이 256 바이트의 메모리를 성공적으로 할당한 후 세그먼트 목록의 상태를
// 보여줍니다. 여기에는 두 가지 요소가 포함되어 있습니다.
// 첫 번째 요소는 752 바이트의 동적 메모리를 차지하는 여유 청크를 나타냅니다.
// 두 번째 요소는 p1에 할당된 메모리를 나타내며 나머지 272바이트를 차지합니다.

// 다음으로 할당된 메모리를 해제하는 알고리즘을 정의하겠습니다.

// - 할당 해제 알고리즘
// 할당 해제 알고리즘의 기본 버전은 매우 간단합니다.
// 할당된 영역에 대한 포인터가 주어지면 바로 앞에 있는 메모리 영역을 보고, 해당 목록 항목을
// 얻습니다. 그런 다음 해당 청크를 사용 가능으로 표시하여 다음에 새 메모리가 요청될 때
// 고려할 수 있도록 합니다.

// 이 버전의 알고리즘은 작동하는 것처럼 보이지만 큰 문제가 있습니다. 목록 항목이 점점 더 많이
// 생성된다는 점입니다. 이렇게 하면 동적 메모리가 파편화되어 더 많은 메모리를 할당하기가
// 점점 더 어려워집니다.

// 이 문제를 해결하기 위해 할당 해제된 청크를 인접한 모든 여유와 청크를 병합합니다.
// 이때 포인터를 따라가면 이전 청크와 다음 청크를 쉽게 확인할 수 있으므로 이중으로
// 연결된 목록이 유용하게 사용됩니다.

// * 구현
// - 이중 링크드 리스트
// 동적 메로리 청크를 크기(구조체 크기 제외)와 사용 여부(즉, 비어 있지 않은지)를 포함하는
// 구조체로 모델링합니다. 이중 링크된 리스트로 만들기 위해 이전 포인터와 다음 포인터를
// 추가합니다. 다음은 코드입니다.

typedef struct dynamic_mem_node {
	uint32_t size;
	bool used;
	struct dynamic_mem_node *next;
	struct dynamic_mem_node *prev;
} dynamic_mem_node_t;

// 다음으로 동적 메모리를 초기화 할 수 있습니다.

// - 초기화
// 동적 메모리를 할당하기 전에 먼저 초기화해야 합니다. 디자인 섹션에서 설명한 대로
// 사용 가능한 메모리 전체를 포함하는 단일 청크로 시작하겠습니다.
// 다음 코드는 4KB의 동적 메모리를 초기화합니다.

#define NULL_POINTER ((void*)0)
#define DYNAMIC_MEM_TOTAL_SIZE 4*1023
#define DYNAMIC_MEM_NODE_SIZE sizeof(dynamic_mem_node_t) // 16

static uint8_t dynamic_mem_area[DYNAMIC_MEM_TOTAL_SIZE];
static dynamic_mem_node_t *dynamic_mem_start;

void init_dynamic_mem() {
	dynamic_mem_start = (dynamic_mem_node_t *) dynamic_mem_area;
	dynamic_mem_start -> size = DYNAMIC_MEM_TOTAL_SIZE = DYNAMIC_MEM_NODE_SIZE
	dynamic_mem_start -> next = NULL_POINTER;
	dynamic_mem_start -> prev = NULL_POINTER;
}

// 메모리 할당 함수의 구현으로 넘어가 보겠습니다.

// - 할당
// 할당 알고리즘 정의에서 복기해 봅시다:
// 먼저 최적의 메모리 블록을 찾습니다. 코드 가독성을 유지하기 위해 알고리즘의 해당 부분에
// 대해 주어진 목록을 살펴보고 가장 작은 여유 노드를 반환하는 별도의 함수
// find_best_mem_block에 생성합니다.

void* find_best_mem_block (dynamic_mem_node_t *dynamic_mem, size_t size) {
	// initialize the result pointer with NULL and an invalid block size
	dynamic_mem_node_t* best_mem_block = (dynamic_mem_node_t* ) NULL_POINTER;
	uint32_t best_mem_block_size = DYNAMIC_MEM_TOTAL_SIZE + 1;
	
	// start looking for the best (smallest unused) block at the beginning
	dynamic_mem_node_t *current_mem_block = dynamic_mem;
	while (current_mem_block) {
		// check if block can be used and is smaller than current best
		if ((!current_mem_block->used) &&
		(current_mem_block->size >= (size + DYNAMIC_MEM_NODE_SIZE)) &&
		(current_mem_block->size <= best_mem_block_size)) {
			// update best block
			best_mem_block = current_mem_block;
			best_mem_block_size = current_mem_block->size;
		}
		
		// move to next block
		current_mem_block = current_mem_block->next;
	}
	return best_mem_block;
}