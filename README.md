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

# fork() 수행 시 Task 구조

## Memory Context
> fork() 수행 시 task 구조의 변화에서는 자식 프로세스는 새로운 pid를 할당받습니다.  먼저 memory context에서 text section은 부모와 자식이 공유를 하고,  <span style ="color:YELLOW">data section/stack section 은 COPY하여 새롭게 만듭니다.</span>

## Hardware Context
> Multitasking에서 Hardware Context는 중요한 부분입니다. 각 프로세스 마다 CPU Register 값을 가지고 있기 때문에 병행처리에서 중요한 개념입니다. <span style="color:Yellow"> Thread에서도 고유의 cpu register 값을 가지고 있습니다.</span><br> -> PCB(PROCESS CONTROL BLOCK)에 상태를 저장합니다.<BR><BR> 문맥 교환 시 주의 사항이 있습니다.<BR>
실질적으로 사용자의 요청을 서비스하는 것이 아닙니다!<BR>
하드웨어의 Support 가 필요하며, 문맥 교환은 시분한 시스템/인터럽트에 의한 문맥교환 또한 이루어집니다.

## User mode/System mode
> 사용자가 작성한 프로그램은 user mode에서 수행됨<br>system call이나 Interrupt는 system mode에서 수행<br>한 프로세스가 user mode와 system mode에서 동시에 수행될 수 없다.<br>system mode에서는 kernel stack을 사용

## Process 와 관련된 System Call
> fork() -> 호출한 프로세스를 복제하여 새로운 프로세스를 생성<br> exec() -> 완전히 새로운 프로그램으로 변형<br>wait() 다른 프로세스가 finish할때까지 기다림<br> exit() 프로세스를 종료<br> fork() return 값에 대해서, Parent Process는 Child Process의 id값을 받고, Child Process는 0을 받는다.<br>Child Process는 Process Group id, session id, controlling terminal을 Parent Process를 따르며 -> 같은 프로세스 그룹에 속함.

### 주의 사항
* Parent와 Child의 실행 순서를 알 수 없다 -> 가정을 하면 안된다.

* 부모 프로세스가 개방한 파일은 자식 프로세스에도 개방 -> read-write 포인터를 공유한다.
* vfork() -> 부모 태스크의 메모리 영역 공유, 즉 자식 프로세스의 변경이 부모 프로세스에게 영향을 준다. 자식 프로세스의 _exit 호출로 종료.

# 새로운 프로그램의 수행 : exec
> 현재 텍스트, 데이터, 스택 등의 영역을 새로운 이미지로 대치<br>
예를 들어 LOGIN SHELL 에서 로그인 성공시 -> fork()를 수행하여 exec()로 새로운 프로그램 실행
 * exec 수행 시 : 개방된 파일 기술자들도 변형된 프로세스에 전달
 * fcntl(fd,F_SETFD,0) -> 0으로 설정한 것은 기존의 오픈된 filedescripter를 이용
 * exit(int status); -> status는 exit status : success는 0, fail은 non-zero
 * wait(int *status); -> return : exit하는 child의 pid, status는 child의 exit_status
 * pid_t waitpid(pid_t pid, int *status, int options); pid는 부모가 기다리고 싶은 child의 pid, -1이면 wait와 동일 options중 <span style="color:Yellow">WNOHANG</span>은 child process가 수행 중일 때 blocking 을 하지 않으면서 상황을 계속 감시할 수 있도록 해주는 옵션이다.

 # Process Group
 > 한 개 또는 그 이상의 Collection, 각 프로세스 그룹은 unique한 PGID를 가지고 있다. <span style="color:Yellow">각 프로세스 그룹은 PID가 PGID와 같은 프로세스 그룹 리더를 가지고 있다.</span> 프로세스 그룹은 그룹 내에 리더가 존재하지 않아도 하나의 프로세스라도 존재하면 그룹은 존재한다.

<br>

# THREAD

> 프로세스는 OS에 의해 생성된다. 프로세스는 프로그램 resource에 대한 정보, 프로그램 실행 상태를 포함하고 있다.<br> 
무엇을 포함하고 있을까?

* Process ID, Process Group ID, user ID, group ID
* Environment
* Working directory, Program instructions, Registers
* Stack, Heap, File descriptors, Signal Actions, Shared libraries, IPC tools

### <span style="color:Yellow">System call, 자주 호출하면 좋지 않다. Why? OS의 Kernel에 서비스를 요청할 때에 호출하는 함숭ㄴ데, 이들은 hardware와 관련된 서비스나 프로세스의 생성/종료, 파일의 I/O등을 처리하며 kernel 모드로 실행되기 때문이다.</span>

> Process, 실행 상태에 있는 프로그램의 인스턴스 자원 소유, Container의 역할, Thread-> 프로세스의 실행단위, 프로세스의 실행 흐름, 주소 공간이나 열린 파일 등 여러 자원을 공유하며, 쓰레드간 공유 자원 접근 시 동기화가 필요하다. 스레드는 프로세스 자원들을 사용하며, 프로세스 내 독립적인 객체로서 실행되고 OS에 의해 Scheduling 된다.

## Thread 특징
* Text Section, Data Section, Heap Section 및 file을 공유한다. 하지만, Stack 영역 및 Cpur Register는 별도로 가지고 있습니다.

## Thread를 사용하는 이유는 무엇일까?
* Parallel Processing의 효율화
* Performance 향상
* 새로운 프로세스 형성은 OS의 Context Switching 호출을 피할 수 없으며 큰 Memory를 차지한다.
* IPC의 cost의 비용이 비싸다.
* Thread들 또한 Client의 Request들을 다룰 수 있다.
* I/O 작업에도 달느 스레드들은 Blocking 되지 않고 일을 수행한다.

# User-level Thread 와 kernel-level Thread
> User-level threads들은 user space에서 실행되는데, runtime library에 의해 생성된다. 따라서 Kernel과 관련된 명령을 수행할 수 없으며, Kernel은 이 Thread들의 존재를 알 수 없다. 장점으로는 User-level 라이브러리가 성능 최적화를 위해 스레드들을 스케줄링 할 수 있으며, library만 존재하면 어떤 OS든 위에서 수행이 가능하며, Context-switch를 피할 수 있다.<br>단점으로는 Kernel이 Thread들로 바라보는게 아닌 single Process로 바라보기에 멀티스레딩의 장점을 가질 수 없다. ex) I/O작업 시, 하나의 프로세스를 보기에 Block 상태를 만든다.

> Kernel-level Thread들은 Kernel이 Thread들의 존재를 알 고 있습니다. 각 스레드 마다 Kernel이 자료구조를 가지고 있으며, 하나의 실행 Context로 바라봅니다. 따라서, 멀티스레딩의 장점을 가지고 있으며 단점으로는 Context Switching 시 발생하는 Overhead가 존재합니다.

# Thread Programming 의 어려움
> 복잡한 알고리즘 디자인과 data 동기화에 대해 Programmer들이 고려하며 수행해줘야한다. Debug와 Test에 대한 어려움도 존재하며 Race Condition에 대해 고려해야 합니다. Thread의 병행 처리에 대해 공유자원에 대해서 발생할 수 있는 문제점들이 존재하며, 운영체제와 관련된 Dead Lock 등 .. 고려해야 할 사항들이 많다.