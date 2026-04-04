cmake_minimum_required(VERSION 3.10)
project(Breakout)

set(CMAKE_CXX_STANDARD 17)

# 1. 璁剧疆Raylib璺緞锛堝叧閿慨鏀癸級
# 鍋囪鎮ㄧ殑raylib-5.0鐩綍缁撴瀯涓嶹indows棰勭紪璇戠増鏈竴鑷?set(RAYLIB_ROOT "${CMAKE_SOURCE_DIR}/raylib-5.0")
include_directories(${RAYLIB_ROOT}/include)  # Windows鐗堟湰澶存枃浠堕€氬父鍦╥nclude鐩綍
link_directories(${RAYLIB_ROOT}/lib)         # Windows搴撴枃浠堕€氬父鍦╨ib鐩綍

# 2. 娣诲姞鍙墽琛屾枃浠讹紙婧愭枃浠堕儴鍒嗕繚鎸佷笉鍙橈級
add_executable(breakout_week2 
    src/main.cpp 
    src/Ball.cpp 
    src/Paddle.cpp 
    src/Brick.cpp
)

# 3. 閾炬帴搴擄紙鍏抽敭淇敼锛氱Щ闄inux搴擄紝娣诲姞Windows绯荤粺搴擄級
if(WIN32)
    # Windows骞冲彴閾炬帴
    target_link_libraries(breakout_week2 
        raylib           # 閾炬帴raylib搴?        opengl32         # OpenGL搴擄紙Windows锛?        gdi32            # 鍥惧舰璁惧鎺ュ彛搴?        winmm            # Windows澶氬獟浣撳簱
        # 绉婚櫎浜哃inux鐗规湁鐨?m, pthread, dl, rt, X11
    )
else()
    # 淇濈暀鍘熸湁鐨凩inux閾炬帴鏂瑰紡浣滀负澶囬€?    target_link_libraries(breakout_week2 raylib m pthread dl rt X11)
endif()


