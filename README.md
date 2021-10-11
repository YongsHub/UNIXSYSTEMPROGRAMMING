# <span style="color:YELLOW">UNIX SYSTEM PROGRAMMING</span>
유닉스 프로그래밍 내용 정리용 저장소

Process? 리눅스에서는 Task라고 부른다.
* 수행중인 프로그램, 프로그램의 수행 환경, 스케줄링 단위 ...
* 프로세스는 CPU time, 메모리, 파일, I/O 장치를 포함한 특정 자원을 필요로 한다.

<BR>

## Memory Context에서 중요한 개념
* text section : 명령어 집합, cpu에서 직접 수행하는 명령어 집합
* data section : *global variable*의 집합
* bss : <u>초기화 되지 않은</u> *global variable*의 집합
* stack : 함수의 return 주소, 지역 변수, 매개변수들의 집합

# <span style="color:YELLOW">Process id</span>
> 모든 프로세스는 unique 한 process ID를 가지고 있습니다.<BR>
getpid() : 현재 프로세스의 id를 return 받습니다.<br>
getppid() : 부모 프로세스의 프로세스 id를 return 받습니다.

# <span style="color:YELLOW">Task 구조 정리</span>
> Text Section -> 프로그램 코드 부분, 프로그램 순서에 따라 메모리 상위 번지로 자랍니다.<br>
Data Section -> 전역 변수, 초기화 된 부분과 초기화 되지 않은 부분을 별도로 관리합니다. data & bss <br>
Stack Section -> 지역 변수, 매개 변수, 복귀 주소  함수 호출 순서에 따라 동적으로 메모리 하위 번지로 자란다.<br>
Heap Section -> malloc() calloc() 등의 인터페이스로 동적 할당되는 부분입니다.

# <span style ="color:YELLOW">User Stack vs Kernel Stack</span>
> 사용자 스택은 지역 변수들, 이전 스택 프레임에 대한 포인터, 함수 호출 수행 후의 리턴 주소, 함수 호출에 사용한 인자들을 관리합니다. 커널 스택은 시스템 호출에 필요한 여러 가지 요소들을 저장하며, 내용은 사용자 스택과 유사합니다.

# Process Context
커널이 관리하는 태스크의 자원과 제어 흐름의 집합 3 부분으로 구성
* SYSTEM CONTEXT
* MEMORY CONTEXT
* HARDWARE CONTEXT
* 문맥 교환 -> Context Switching, 프로세스 스위칭이라고도 한다.
> 프로세스 마다 Context를 가지고 있는데, 이것을 초기화 하는데에 있어서 많은 시간을 소요한다. 즉 -> Overhead가 크다.
task structure => Process Control Block 에서 system context의 파일이라던지, hardware context, memory context를 모두 관리한다고 생각하면 좋다.

## Thread라는 개념이 나온 이유?
* 프로세스 하나를 만드는데 오버헤드가 크기에 Thread라는 개념이 나왔습니다. 프로세스 내에 Text Section, Data Section, Heap Section을 공유하면서 Stack은 각각 새로 만들며, hardware context를 별도로 생성하면 됩니다.

### Task 의 연결 구조는 Double Linked List로 구성


