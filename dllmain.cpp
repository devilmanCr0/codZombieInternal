#include "tools.h"
#include "dxDraw.h"
#include "Memory.h"
#include <vector>

//fixed crashes relating to permissions
//next implement the ent list structure
double PI = 3.14159265358;

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
void* d3d9Device[119];
BYTE EndSceneByte[7]{ 0 };
tEndScene oEndScene = nullptr;
LPDIRECT3DDEVICE9 pDevice;
DXD::Draw* DrawingDevice;
uintptr_t moduleAddy;
HookingAuto* GraphicsHook;

byteSequence recoilInstructionList[]
{ { 0x3669EC,2, (BYTE*)"\x89\x10"}
 ,{0x3696BD, 2, (BYTE*)"\x89\x01"}
 ,{0x366183, 2, (BYTE*)"\x89\x10"}
 ,{0x3686EF, 3, (BYTE*)"\x89\x56\x3C"}
 ,{0x36752E, 3, (BYTE*)"\x89\x4E\x3C"}
 ,{0x3673DE, 3, (BYTE*)"\x89\x4E\x3C"}
 ,{0x366CCC, 6, (BYTE*)"\x81\x00\xF4\x01\x00\x00"}
 ,{0x3686E4, 3, (BYTE*)"\x89\x4E\x3C"}
 ,{0x3686D5, 3, (BYTE*)"\x89\x46\x3C"}
 ,{0x3663EA, 3, (BYTE*)"\x89\x4E\x3C"}
 ,{0x369835, 6, (BYTE*)"\xC7\x01\x01\x00\x00\x00"}
 ,{0x3669DE, 2, (BYTE*)"\x89\x01"}
 ,{0x369173, 2, (BYTE*)"\x89\x10"}
 ,{0x36708F, 3, (BYTE*)"\x89\x56\x3C"}
 ,{0x36A1E6, 3, (BYTE*)"\x89\x46\x3C"}
 ,{0x3670B8, 3, (BYTE*)"\x89\x56\x3C"}
 ,{0xB06F3 , 3, (BYTE*)"\xD9\x5E\x14"}
};

namespace hackToggles
{
    bool bGod{ false };
    bool bGodRunOnce{ true };

    //Currently Broken
    bool bRapidfire{ false };
    bool bRapidfireRunOnce{ true };

    //Currently Partially Broken
    bool bESP{ false };

    //Currently Shit
    bool bAimbot{ false };

    bool bTriggerBot{ false };

    bool bMoney{ false };

    bool bShutdown{ false };

    bool bAimbotRunOnce{ false };
   


    //part of ent List *singleplayer
    bool bFreezeZombies{ false };
    bool bFreezeSwitch{ true };
    bool bKillAll{ false };

    //I havent figured out how to fly
    //I havent figured out how to aquire different weapons
}

ZombieZ* getZombie(int index)
{
    return (ZombieZ*)*(uintptr_t*)(signatures::entityZombieList + (index * 0x8C));
}

PlayerStats* getPlayerStats()
{
    return (PlayerStats*)(moduleAddy + (uintptr_t)signatures::ClientState::localPlayerStats);
}

PlayerMovement* getPlayerMovement()
{
    return (PlayerMovement*)((uintptr_t)signatures::localPlayerMovement);
}

//ZombieZ* getZombieTrigger(PlayerMovement* player)
ZombieZ* getZombieTrigger()
{
    //Replicated with IDA, uses this as a lookup to see what 
    //Entity you're looking at
    //PLayerMovement+1C80
    //module+16794EC
    uintptr_t* firstBase = (uintptr_t*)*(uintptr_t*)(0x1A794EC + (*((short int*)0x1C0A7C0) * 0x34C));

    if (firstBase)
    {
        return (ZombieZ*)*firstBase;
    }
    return 0x0;
}

bool isValidZombie(ZombieZ* zombie)
{
    if (!zombie)
    {
        return false;
    }

    if (!((zombie->ZombID > 400) && (zombie->ZombID < 800)))
    {
        return false;
    }

    if (zombie->Health <= 0)
    {
        return false;
    }

    return true;
}


ZombieZ* getClosestZombie(uintptr_t moduleAddy)
{
    Vector3 localPlayerPos = getPlayerMovement()->playerPosition;
    float closestDistance = 1000000;
    int closestDistanceIndex = -1;
    int maxZombies = *(int*)(moduleAddy + (uintptr_t)signatures::ClientState::zombieCount);
    for (int i{ 0 }; i < maxZombies; i++)
    {
        ZombieZ* zombie = getZombie(i);

        if (!isValidZombie(zombie))
        {
            continue;
        }

        //we already have a builtin function to get distance, just do localPlayerPos.getDistancee(zombie->ZombPosition);
        float currentDistance = localPlayerPos.Distance(zombie->ZombPosition);
        if (currentDistance < closestDistance)
        {
            closestDistance = currentDistance;
            closestDistanceIndex = i;
        }

    }
    if (closestDistanceIndex == -1)
    {
        return NULL;
    }
    return getZombie(closestDistanceIndex);
}

void aimAt(PlayerMovement* localPlayer, ZombieZ* zombie, Vector2* localAngle, float offsetOne, float offsetTwo)
{
    Vector3 playerPos = localPlayer->playerPosition;
    Vector3 zombiePos = zombie->ZombPosition;

    Vector3 deltaVec = { zombiePos.x - playerPos.x ,  zombiePos.y - playerPos.y , zombiePos.z - playerPos.z };
       
    //Yep, this means its an imp bastard, shoot it at a lower angle
    if (zombie->Distinguish == 24)
    {
        deltaVec.z -= 25;
    }


    float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

    float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
    float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

    if (pitch >= -85 && pitch <= 85 && yaw >= -180 && yaw <= 180)
    {
        //my calculations are correct, we just cant find the right values to modify
        //find the correct initial length
        //float difference = 61.4;

        //Because for some reason we have values that can overflow infinitely, we need 
        //to accomadate it using an offset from its current initial value and 


        //Gooning
        localAngle->x = (pitch+2)+offsetOne;
        localAngle->y = (yaw+offsetTwo);
    }
}

void triggerShoot()
{
    ZombieZ* zombie = getZombieTrigger();
    if (isValidZombie(zombie))
    {
        //BlackOps.exe + 24D9230
        int i = 0;
        while (i < 10000)
        {
            *((int*)0x028D9230) = 1;
            i++;
        }
    }
    *((int*)0x028D9230) = 0;
}

void Draw()
{
    if (hackToggles::bESP)
    {
        int maxZombies = *(int*)(moduleAddy + (uintptr_t)signatures::ClientState::zombieCount);
        for (int i{ 0 }; i < maxZombies; i++)
        {
           ZombieZ* zombie = getZombie(i);

           if (!isValidZombie(zombie))
           {
               continue;
           }

            

            Vector3 center = zombie->ZombPosition;

            //center.z = adjustment
            float* projectionMatrix = (float*)((uintptr_t)moduleAddy + signatures::ClientState::graphicSettings + signatures::ClientState::projectionMatrix);

            //Resolution
            int ResolutionX = *(int*)0x0BA6960;
            int ResolutionY = *(int*)0x0BA6964;

            Vector3 screen;
            if (DrawingDevice->WorldToScreen(center, screen, projectionMatrix , ResolutionX, ResolutionY))
            {
                DrawingDevice->DrawESP(getPlayerMovement(), zombie, screen);
            }

            
        }

    }



}



void APIENTRY hktEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (pDevice != o_pDevice)
    {
        pDevice = o_pDevice;
        DrawingDevice->Initalize(pDevice);
    }
    
    Draw();
    //DrawingDevice->DrawFilledRect(50, 50, 50, 50, rgb::red);

    oEndScene(pDevice);
}

DWORD WINAPI BOZomb(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    
    

    bool truth{ true };
    DrawingDevice = new DXD::Draw(d3d9Device, sizeof(d3d9Device), truth) ;
    moduleAddy =  (uintptr_t)GetModuleHandle(L"BlackOps.exe") ;

    std::cout << "Hello, welcome to the spludge zone" << std::endl;

    std::cout << "I for God\n";
    std::cout << "O for Rapid Fire\n";
    std::cout << "J for ESP - BAD\n";
    std::cout << "K for aimbot";
    std::cout << "N for money\n";
    std::cout << "M for freeze zombies - SUPER BROKEN\n";
    std::cout << "P for kill all - BROKEN\n ";
    std::cout << "L for Trigger Bot\n";

    //BlackOps.exe+24D9230
    //Set to 1 to fire, 0 otherwise


    if (truth)
    {
        memcpy(EndSceneByte, (char*)d3d9Device[42], 7);

        GraphicsHook = new HookingAuto( (BYTE*)d3d9Device[42], (BYTE*)hktEndScene, (BYTE*)&oEndScene, 7 );

        GraphicsHook->Toggle();
 
    }
    else {
        std::cout << "It Broke.... Sorry " << std::endl;
    }
    while (!(GetAsyncKeyState(VK_END) & 1))
    {
        // [5B ]5D ;3B '27 ,2C .2E /2F
        
        if ((GetAsyncKeyState(VK_END) & 1))
        {
            if (!hackToggles::bShutdown)
            {
                hackToggles::bShutdown = 1;
                hackToggles::bGod = false;
                hackToggles::bRapidfire = false;        
                hackToggles::bESP = false;
                hackToggles::bAimbot = false;
                hackToggles::bMoney=  false;
              
            }
         
        }
        
        
        // I
        if (GetAsyncKeyState(0x49) & 1)
        {
            hackToggles::bGod = !hackToggles::bGod;
        }

        // O
        if (GetAsyncKeyState(0x4F) & 1)
        {
            hackToggles::bRapidfire = !hackToggles::bRapidfire;
        }

        // J
        if (GetAsyncKeyState(0x4A) & 1)
        {
            hackToggles::bESP = !hackToggles::bESP;
        }

        // K
        if (GetAsyncKeyState(0x4B) & 1)
        {
            hackToggles::bAimbot = !hackToggles::bAimbot;
        }

        // N
        if (GetAsyncKeyState(0x4E) & 1)
        {
            hackToggles::bMoney = !hackToggles::bMoney;
        }

       // M
        if (GetAsyncKeyState(0x4D) & 1)
        {
            hackToggles::bFreezeZombies = !hackToggles::bFreezeZombies;
        }

        // P
        if (GetAsyncKeyState(0x50) & 1)
        {
            hackToggles::bKillAll = !hackToggles::bKillAll;
        }

        // L
        if (GetAsyncKeyState(0x4C) & 1)
        { 
            hackToggles::bTriggerBot = !hackToggles::bTriggerBot;
        }

        if (hackToggles::bGod)
        {
            if (hackToggles::bGodRunOnce)
            {
                memoryPatch(2, (BYTE*)(moduleAddy + 0x3DADCE));

                hackToggles::bGodRunOnce = false;
            }
            
        }
        else
        {
            if (!hackToggles::bGodRunOnce)
            {
                memoryAlloc(2, (BYTE*)(moduleAddy + 0x3DADCE), (BYTE*)"\x2B\xC7");

                hackToggles::bGodRunOnce = true;
            }
        }

        if (hackToggles::bRapidfire)
        {
            if (hackToggles::bRapidfireRunOnce)
            {
          
                // patching here
                for (byteSequence daBytes : recoilInstructionList)
                {

                    memoryPatch(daBytes.byteSize, (BYTE*)((uintptr_t)moduleAddy + daBytes.bytes));


                }


                hackToggles::bRapidfireRunOnce = false;
            }
        }
        else
        {
            if (!hackToggles::bRapidfireRunOnce)
            {
                // patching here 
                for (byteSequence daBytes : recoilInstructionList)
                {
                    memoryAlloc(daBytes.byteSize, (BYTE*)((uintptr_t)moduleAddy + daBytes.bytes), (BYTE*)daBytes.byteString);
                }


    
                hackToggles::bRapidfireRunOnce = true;
            }
        }

        
        
        if (hackToggles::bMoney)
        {
            *(int*)(moduleAddy + signatures::ClientState::inGameCurrency) += 500;
            hackToggles::bMoney = false;
        }



        if (hackToggles::bAimbot)
        {

            

            ZombieZ* closestZombie = getClosestZombie(moduleAddy);
            if (closestZombie)
            {
                PlayerMovement* localplayer = getPlayerMovement();
                Vector2* localplayerAngle = (Vector2*)(moduleAddy + signatures::ClientState::yawpitchAddress);

                static float offsetOne = localplayerAngle->x - localplayer->pitchYaw.x;
                static float offsetTwo = localplayerAngle->y - localplayer->pitchYaw.y;
                if (hackToggles::bAimbotRunOnce)
                {
                    offsetOne = localplayerAngle->x - localplayer->pitchYaw.x;
                    offsetTwo = localplayerAngle->y - localplayer->pitchYaw.y;

                    hackToggles::bAimbotRunOnce = false;
                }

                aimAt(localplayer, closestZombie, localplayerAngle, offsetOne, offsetTwo);
            }


        }
        else {
            hackToggles::bAimbotRunOnce = true;
        }

        if (hackToggles::bTriggerBot)
        {
            triggerShoot();
        }


        if (hackToggles::bFreezeZombies)
        {
            Vector3 prevCoords = { 0 , 0 , 0 };
            
            int maxZombies = *(int*)(moduleAddy + (uintptr_t)signatures::ClientState::zombieCount);
            for (int i{ 0 }; i < maxZombies; i++)
            {
                ZombieZ* zombie = getZombie(i);

                if (!zombie)
                {
                    continue;
                }

                if (!((zombie->ZombID > 600) && (zombie->ZombID < 800)))
                {
                    continue;
                }

                if (zombie->Health <= 0)
                {
                    continue;
                }

                if (hackToggles::bFreezeSwitch)
                {
                    Vector3 coords = zombie->ZombPosition;
                    prevCoords = coords;
                    hackToggles::bFreezeSwitch = false;
                }
                
                zombie->ZombPosition = prevCoords;
               
            }
        }
        else
        {
            hackToggles::bFreezeSwitch = true;
        }

        if (hackToggles::bKillAll)
        {
            int maxZombies = *(int*)(moduleAddy + (uintptr_t)signatures::ClientState::zombieCount);
            for (int i{ 0 }; i < maxZombies; i++)
            {        
                ZombieZ* zombie = getZombie(i);

                if (!zombie)
                {
                    continue;
                }

                if (!((zombie->ZombID > 600) && (zombie->ZombID < 800)))
                {
                    continue;
                }

                if (zombie->Health <= 0)
                {
                    continue;
                }
                
                //changed from 0 to -1 to see if it would force a kill
                zombie->Health = -1;
            
            }
        }

        if (hackToggles::bShutdown)
        {
            break;
        }
    }



    GraphicsHook->Disable();

    Sleep(2);
    delete GraphicsHook;

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hThread = nullptr;
        hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)BOZomb, hModule, 0, nullptr);
        if (hThread)
        {
            CloseHandle(hThread);
        }
    }
    [[fallthrough]];
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

