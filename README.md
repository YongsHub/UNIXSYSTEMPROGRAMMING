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

# Session
>각 프로세스 그룹은 하나의 세션에 소속합니다. 또한 각 세션은 하나의 제어 단말기 즉 <span style="color:Yellow">Controlling Terminal</span>을 가집니다. 한 세션은 하나의 foreground process group과 하나 또는 그 이상의 background process group으로 구성됩니다.

<BR><BR>

## Daemon Process
setsid를 사용해서 만들어진 새로운 세션내에는 background process group만 존재한다고 생각하면 이해하기 편하다. 그리고 일반적으로 서버로 동작하는 process인 경우 setsid를 이용하여 새로운 세션을 생성하는데 보통 하나의 process로 존재하게 되는데
이 프로세스를 daemon rocess라고 부른다고 생각하면 이해하기 쉽다. 즉 background process group에 존재하는 background process가 daemon process라고 생각하면 좋다.
* Daemon Process는 controlling terminal을 갖지 않는 프로세스
* setsid()를 사용하여 생성된다.

## setsid()
>setsid를 호출하는 프로세스는 새로운 세션의 리더이다.<br>함수 호출하는 프로세스는 새로운 프로세스 그룹의 그룹 리더 입니다.<br>위에서 설명했듯, Daemon Process로 생각하면 이해하기 쉬우며 Controlling Terminal을 갖지 않습니다.<br>Process Group ID & Seesion ID는 호출한 프로세스 PID로 맞추어집니다.<BR>호출한 프로세스가 새로운 프로세스 그룹과 새로운 세션의 유일한 프로세스이다.

## getsid()
>getsid(pid_t pid); -> 프로세스 ID를 매개변수로 이용하면 그 프로세스가 속한 session ID가 return 됩니다. getsid(0);은 calling process의 세션 ID가 return 됩니다.

## Controlling Terminal
>controlling terminal은 보통 터미널 장치이다. 세션은 하나의 controlling terminal을 가질 수 있습니다. 세션 리더는 controlling porcess(제어 프로세스)라 불리며 controlling terminal과 연결됩니다. 또한 세션 내  하나의 foreground group과 하나 이상의 background process group이 존재하는데, hang-up 시그널이 세션 리더 즉 제어 프로세스에게 보내집니다.
<br>

# THREAD

> 프로세스는 OS에 의해 생성된다. 프로세스는 프로그램 resource에 대한 정보, 프로그램 실행 상태를 포함하고 있다.<br> 
무엇을 포함하고 있을까?

* Process ID, Process Group ID, user ID, group ID
* Environment
* Working directory, Program instructions, Registers
* Stack, Heap, File descriptors, Signal Actions, Shared libraries, IPC tools

### <span style="color:Yellow">System call, 자주 호출하면 좋지 않다. Why? OS의 Kernel에 서비스를 요청할 때에 호출하는 함수인데, 이들은 hardware와 관련된 서비스나 프로세스의 생성/종료, 파일의 I/O등을 처리하며 kernel 모드로 실행되기 때문이다.</span>

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


# POSIX Thread Programming

요즘 CPU는 MultiCore를 이용하고 있고, 멀티스레드 프로그래밍은 이러한 <span style="color:yellow">멀티코어의 장점을 극대화 할 수 있다.</span> Serial Processing, Interleaving Processing, Overlapping 즉, Parallel Processing에 대해 이해하고 있어야 한다.
* 우리가 알아야 할 점은 text/data/heap/file 들을 공유하기 때문에 Race Condition 발생확률이 높다. 따라서 이 부분을 Serialize하게 만들어야 한다.

* Pthread(POSIX Thread)는 SingleUnixSystemVersion에 pthread가 들어있다.
* Compile시, -lpthread를 함께 해야 합니다.
* 리눅스에서의 Pthread는 Kernel level thread 입니다.

## Pthread API
* 기본적으로 CREATING, DETACHING, JOINING 과 같은 기본
* 동기화를 위한 Mutex 동기화 함수로서 -> Creating, Destroying, locking and unlocking이 존재한다.
* Condition Variable, 스레드들간에 mutex를 공유하며 의사소통하기 위함 : create, destroy, wait 등등
* POSIX는 C Language에 정의되어 있습니다.

> Pthread_create : thread를 생성<br>Pthread_exit : process를 종료하지 않고, Thread만 종료<br>Pthread_kill : thread에게 signal을 보냅니다. <br> Pthread_detach : thread가 자원을 해제하도록 설정 <br> Pthread_equal: 두 thread ID가 동일한지 검사 <br> Pthread_self : 자신의 thread ID를 얻음 <BR> Pthread_cancel : 다른 thread의 수행을 취소

pthread_create(thread,attr,start_routine,arg)의 return 은 각 스레다마다 ID를 받는다.
* attr: thread 속성 set, NULL은 default values
* start_routine : 실행을 시키고자 하는 함수명
* arg : start_routine의 매개변수, void 타입의 포인터로써 처리되야 한다. 여러 변수들인 경우, 구조체로 묶어서 void pointer로 보내야 합니다.

## Pthread가 종료되는 경우
* starting function으로부터 return
* thread가 pthread_exit 함수를 호출했을 때
* thread가 다른 pthread_cancel로부터 취소되었을 때
* entire process가 terminated되었을 때 / exec() or exit
* pthread_exit() 루틴은 file을 닫지 않습니다. thread 안에서 오픈된 파일은 스레드가 종료된 이후에도 open된 상태로 남아있습니다.<br>

## Manager/Worke Model
스레드를 생성하는 경우에도 오버헤드가 존재하기 때문에 Thread Pool을 이용할 수 있습니다. manager는 run-time overhead를 절약할 수 있습니다. 스레드를 생성해두고 work queue에 저장, block된 상태로 대기를 하다가 signal을 통해서 깨울수 있다.

## Thread Safeness
> Thread를 생성하는 것은 매우 쉬운 Part이다. 하지만 데이터를 공유하는 것은 어렵습니다. Global Variable을 공유하는 것은 위험합니다. 두개 스레드가 같은 변수를 수정하려고 하면 Race Condition이 발생할 가능성이 있기 때문입니다. 예를 들어, Application은 몇몇 스레드를 생성할 수 있는데, 같은 library routine을 호출한다고 가정합니다. library routine은 global structure나 메모리 위치를 접근할 수 있다/각 스레드는 이 루틴을 부를 수 있고, 동시간에 global structure 및 메모리 위치를 수정할 수 있습니다. 따라서, <span style="color:yellow">동기화하지 않으면 문제가 발생합니다.</span>

## Thread Synchronization Tool
> pthread_join 함수와 Mutex 변수를 활용하여 Lock<BR>Reader/Writer exclusion -> Read에 대해서는 여러 개 있어도 되지만 Write일 경우 상호배제가 필요하다.

## Mutex Variable
> 보호하고 싶은 자원을 위해 Mutex Variable을 생성하고 초기화해야함.<br>스레드가 자원을 접근할 때, pthread_mutex_lock을 이용하여 자원의 mutex를 lock 해야한다. 오직 하나의 스레드만 mutex lock을 할 수 있고, 나머지는 기다려야만 한다.<br>스레드가 자원에 대한 작업이 끝났을 때, pthread_mutex_unlock을 호출하여 mutex를 unlock할 수 있다.

* Mutex variable은 선언 후, 반드시 초기화 되어야 한다.
## 초기화 2가지 방법 예제
* pthread_mutex_t mymutex = THREAD_MUTEX_INITIALIZER;
* pthread_mutex_init(mutex, attr);
* mutex는 처음에는 unlock상태 이다.
> pthread_mutex_lock() 정해진 mutex variable에 의해 lock을 할 수 있다. 만약, <span style="color:yellow">mutex가 다른 스레드에 의해 이미 locked되었다면, 이것은 호출한 스레드를 block합니다.(mutex가 unlock될때까지)</span>

## pthread_mutex_trylock()
>다른 스레드가 이미 mutex를 hold하고 있을때 mutex접근을 시도하면 block되지 않고 바로 return 되는데 lock을 획득할때까지 Running 상태로 머무릅니다. spin lock이라고도 부릅니다.<br>일반 lock일 경우, Block 상태로 대기하다가 Ready상태로 돌아간 후, 실행되는 Overhead가 존재하지만, 멀티스레드 환경에서 Block 상태로 대기가 아닌 Running상태에서 lock을 획득할때까지 계속 return하게 된다.

## Condition Variables
>Thread간의 synchronization을 위해 사용합니다.<br>어떤 기다리는 조건이 만족되었음을 다른 thread에게 알림으로써 동기화<br>Condition variable은 항상 mutex lock과 연계해서 사용<br>Condition variable은 pthread_cond_t type 선언


<br><br>
# Waiting/Signalling On Conditional Variables

>pthread_cond_wait(condition,mutex);<br>pthread_cont_timedwait(condition,mutex);<br>pthread_cond_singal(condition);<br>pthread_cond_broadcast(condition);

>pthread_cond_wait()는 condition이 signal 될때까지 block되는 함수이다.<span style="color:yellow">이 함수는 반드시 mutex가 lock된 상태에서 호출되어야 하며, 호출되면 자동으로 mutex를 unlock하고 대기합니다.</span> Signal에 의해 깨어날 때에는 mutex는 다시 lock이 걸린 상태입니다.<br>pthread_cond_signal()은 pthread_cond_wait()로 대기하는 thread를 wakeup하는 함수입니다. 이 함수 또한 lock된 상태에서 호출되어야 합니다.<br>pthread_cond_broadcast()는 condition에서 대기하는 여러 thread를 모두 깨울 때 사용합니다.<br>pthread_cond_wait()에 의한 대기 thread가 없는 상태에서의 pthread_cond_signal() 호출은 no action

* Condition Variable에서 여러 쓰레드들이 기다리고 있다면, pthread_cond_signal call을 받는다면 누가 먼저 깨어날까?
* Scheduling 우선순위 또는 First-in First-Out

# Reader/Writer Lock

>initialize read/write lock => pthread_rwlock_init<br>read lock 얻는 방법<br>pthread_rwlock_rdlock<br> pthread_rwlock_tryrdlock<br>pthread_rwlock_timedrdlock<br>write lock을 얻는 방법<br>pthread_rwlock_wrlock<br>pthread_rwlock_trywrlock<br>pthread_rwlock_timedwrlock
<br>read/write lock 푸는 방법<br>pthread_rwlock_unlock<br>read/write lock 제거방법<br>pthread_rwlock_destroy

## Mutex에 대해....
유명하고 이해하기 쉽지만, 프로그래머들이 unlock에 대해 까먹거나 다른 스레드가 lock을 사용했다는 것을 잊거나, mutex를 포함하고 있는 프로그램들을 이해하기 어려운 점들도 있습니다.

<br>

# Semaphore
mutex는 busy-waiting을 초래하고, 오직 "상호 배제"에 사용되며 공유를 할 수 없다는 점, 공평성 또한 보장하기 어렵다.
>2가지 속성으로 변수를 공유하는데, 정수 값으로 n으로 지정한 스레드만큼 세마포어를 공유할 수 있습니다. 세마포어에 대해 기다리는 스레드들의 list가 존재, FIFO를 보장합니다.



## Unnamed Semaphores과 Named Semaphores
* Unnamed는 스레드들과 연관 있는 프로세스에게 동기화를 제공하며, sem_init을 사용합니다.
* Named는 연관있는 스레드들과 프로세스들 뿐만 아니라 연관없는 프로세스들에게도 동기화를 제공합니다. Kernel에 영속적(데이터를 생성한 프로그램 실행이 종료되어도 사라지지 않는 데이터 특성) 이며, sem_open을 사용합니다.
> sem_init(sem_t* sem, int pshared, unsigned int value);<br>pshared값이 0이면 프로세스 내 스레드들만 이용가능하다.<br>int sem_wait(sem_t *sem);<br>lock을 거는 것<br>int sem_post(sem_t *sem);<br>unlock 해주는 것 <br>int sem_destroy(sem_t *sem);

<br>


# Time Management
* time 명령어
* Epoch 이후의 흐른 초 표시 time() 함수
* 날짜와 시간 표시 (ctime() 함수)
* gettimeofday() ms 단위 표시 가능한 함수
* clock_gettime() ns 단위 즉 더 세분화된 함수

>$time 프로그램명<br>출력되는 내용 : Real, User, System <br>Real은 프로그램 시작해서 종료될 때 까지 걸린 시간을 의미<br>User은 User 영역에서 실행된 시간 <br> System은 kernel 영역에서 실행된 시간<br> 일반적으로 Real > User + System 이지만 병렬적으로 처리될 때 System의 Overhead가 존재하므로 Real < User + System인 경우도 있다.

### Epoch : 1970년 1월 1일 0시 0분 0초
time() 함수 시간을 Epoch 이후의 초 단위로 저장하고, 할는 정확히 86,400초로 나누어서 계산합니다. 이 때, difftime() 함수는 두 달력시간의 차를 계산하는 함수입니다.

* ctime(const time_t * clock) 함수 : Epoch 이후의 시간을 초 단위 시간을 매개변수로 취해, 시간을 나타내는 구성요소로 구성된 구조체를 문자열로 저장하는데 날짜와 시간표시가 됩니다.

* clock_gettime() 함수 : Epoch 이후의 시간을 초와 나노 초로 표시되는 시간으로 출력한다.

# pipe를 이용한 프로세스간 통신
초창기에는 물리 메모리에 대해 메모리 접근이 가능했다. 옛날에는 IPC 기법을 이런식으로 사용했는데, 각 프로세스들이 값을 바꾸거나 다른 결과를 초래하거나, TEXT SECTION을 수정하거나, OS를 건들인다면? 심각한 문제를 만들 것이다.<BR>이후, 메모리 영역을 Memory Protection을 확실히 하였습니다. 다른 영역을 접근하려고 하면 Memory Fault/Segment Fault를 일으키며 따라서 나온 것이 IPC(Inter Process Communication)입니다.<br> pipe(file)을 이용하는 방법이 대표적으로 있는데, 오래 걸린다는 점이 있다.<br>진화하면서, System V에서 Advanced IPC -> Shared Memory 초창기와 다르게 os에 요청하여 허용이 되는 경우에만 메모리 공유(물리메모리를 논리적으로) 제어된 형태로 접근합니다. 또는 Message Passing 기법도 있습니다. (Message Queue를 활용하여 Key값으로 queue를 구분)

>파이프 : 한 프로세스를 다른 관련된 프로세스에 연결시켜주는 단방향의 통신 채널입니다. 예를 들어 pr doc | lp 명령어는 
<br>pr doc > tmpfile <br> lp > tmpfile <br> rm tmpfile과 같이 처뢰되며,자료의 흐름은 자동적으로 보이지 않게 처리함 int pipe(int filedes[2]); Return filedes[0] : read pipe filedes[1] : write pipe

## 파이프의 크기
* 유한합니다. 파이프에 들어있는 자료가 일정량을 초과하면 그 후의 write는 봉쇄됩니다.
* 최소 512Byte
* 대부분의 시스템은 훨씬 큰 파이프 크기를 가짐

## 파이프의 read, write는 프로세스를 지연시킬 수 있음.
* 파이프가 꽉 차면 write는 수행이 중단된다.(BLOCK 상태)
* 프로세스에 의해 자료가 읽혀져 파이프에 충분한 공간이 마련될때 까지 수행이 일시 중단된다.
* 파이프가 비어있으면 read 또한 block 됩니다.
* 부모와 자식 간의 파일 디스크립터는 공유한다는 사실을 잘 기억하자!

## Pipe 닫기
* write 파이프를 닫았을 때, 파이프에 대한 read는 파일의 끝에 도달한 것과 마찬가지로 0을 return
* read 파이프를 닫았을 때, 이 파이프에 write를 시도하는 모든 프로세스는 커널로부터 SIGPIPE 시그널을 받습니다. 이러한 경우, write 하는 프로세스는 SIGPIPE의 Default Action으로 termination 합니다.

## Non-Blocking 구현하는 방법
* fctnl(열려 있는 파일에 대한 설정)
* fctnl(filedes, F_SETFL, O_NONBLOCK)
BLOCK 되지 않은 상태로 계속 TRY할 수 있다는 점이 있다.

## 다수의 파이프를 취급하기 위한 Select 사용하기
* 우리는 흔히 I/O Multiplexing 이라고 들어봤을 것이다. Server-Client 모델에서 여러 Client가 실시간으로 접근할 때 서버의 입장에서 한번에 처리해주지 못하고 Sequential 하게 처리하기 때문에 새로운 요청이 들어와도 순차적으로 다음 것을 처리 후 순서대로 처리한다. Network에서는 Socket을 이용하여 메시지를 송수신하지만, 우리는 UNIX에서 Pipe를 Socket 처럼 생각하면 된다. 따라서 다수의 파이프를 취급하기 위해 select를 사용하면 된다.(파일, terminal, FIFO, Socket등에 사용가능)

* int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval * timeout);
* nfds : select 호출에서 검새해야할 파일 기술자의 개수
* readfds, writefds, errorfds : fd_set 비트마스크에 대한 포인터
* timeout: struct timeval에 대한 포인터

>비트 조작 매크로<br>void FD_ZERO(fd_set *fdset); fdset이 가리키는 마스크를 초기화 합니다.<br>void FD_SET(int fd, fd_set *fdset); fdset에 fd를 넣어서 1로 설정<BR>int FD_ISSET(int fd, fd_set *fdset); fdset이 가리키는 마스크내의 비트, fd가 설정되어 있는지 확인하기 위해서<br>void FD_CLR(int fd, fd_set *fdset); fdset이 가리키는 fd에 대해 비트를 0으로 한다.

## Unnamed pipe의 단점
* 부모와 자식 프로세스들을 연결하는 데만 사용할 수 있다.
* 영구히 존재할 수 없으며, Temperory File로 이용함.

## Named pipe
* 임의의 두 프로세스를 연결할 수 있다.
* 영구적입니다.
* UNIX 파일 이름을 부여 받습니다. <br>EX) prw-rw-r-- 1 ben usr 0 Aug 1 21:05 channel
* FIFO 생성 명령어: /etc/mknod channel p
* 명령어 수준에서 FIFO를 사용하는 예<BR> cat < channel & <br>ls -l > channel; wait