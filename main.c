#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <objbase.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

typedef HRESULT(CALLBACK* CoInit)(void*);
typedef void(CALLBACK* CoUnit)(void);
typedef HRESULT(CALLBACK* CoCrIn)(CLSID*, LPUNKNOWN, DWORD, IID*, void**);

/** Declaramos las GUID's de cada interfaz a utlizar **/
const CLSID CLSID_MMDeviceEnumerator =  {0xBCDE0395, 0xE52F, 0x467C, 0x8E,0x3D, 0xC4,0x57,0x92,0x91,0x69,0x2E};
const IID IID_IMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, 0xA7,0x46, 0xDE,0x8D,0xB6,0x36,0x17,0xE6};
const IID IID_IAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, 0x97,0x22, 0x0C,0xF7,0x40,0x78,0x22,0x9A};

int function_volume(IAudioEndpointVolume* control);
void bucle_function_volume(IAudioEndpointVolume* control);

int main()
{

    HANDLE getDll = LoadLibrary("Ole32");
    CoInit coInit = (CoInit)GetProcAddress(getDll, "CoInitialize");
    CoUnit coUnit = (CoUnit)GetProcAddress(getDll, "CoUninitialize");
    CoCrIn coCreateIns = (CoCrIn)GetProcAddress(getDll, "CoCreateInstance");

    HRESULT resp;

    IMMDeviceEnumerator* i_deviceEnum = NULL;
    IMMDevice* i_device = NULL;
    IAudioEndpointVolume* i_audioend = NULL;

    /** Inicamos libreria COM **/
    if((resp = coInit(NULL)) == S_OK){

	/** Obtenemos la interfaz MMDeviceEnumerator **/
        if((resp = coCreateIns(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &IID_IMMDeviceEnumerator, (void**)&i_deviceEnum)) == S_OK){

	    /** De la interfaz MMDeviceEnumerator llamamos al metodo GetDefaultAudioEndpoint para obtener la interfaz MMDevice **/
            if((resp = i_deviceEnum->lpVtbl->GetDefaultAudioEndpoint(i_deviceEnum, 0, 0, &i_device)) == S_OK){

		/** De la interfaz MMDevice llamamos al metodo Activatet para obtener la interfaz IAudioEndpointVolume **/
                if((resp = i_device->lpVtbl->Activate(i_device, &IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void**)&i_audioend)) == S_OK){

                    if(function_volume(i_audioend) == 1){
                        printf("Error en opciones de volumen");
                        return 1;
                    }

                }else{
                    printf("Error al obtener interfaz IAudioEndpointVolume");
                    return 1;
                }

            }else{
                printf("Error al obtener interfaz IMMdevice");
                return 1;
            }

        }else{
            printf("Error al obtener interfaz IMMDeviceEnumerator");
            return 1;
        }
    }else{
        printf("Error al iniciar libreria COM");
        return 1;
    }

    coUnit(); 
    FreeLibrary(getDll);
    return 0;
}


/** Opciones para el control de volumen **/
int function_volume(IAudioEndpointVolume* control){
    int opcion, resp, salir;
    HRESULT ok;
    float volumen;
    boolean mute;

    resp = 1;

    printf("Ingrese opcion: \n 1 = Establecer volumen \n 2 = Obtener volumen actual \n 3 = Mute? \n 4 = Quitar o establecer mute \n 5 = Salir \n\n");
    scanf("%d", &opcion);


    switch(opcion){
        case 1:
            printf("Ingrese volumen de 0 a 100%: ");
            scanf("%f", &volumen);
            if(volumen >= 0 && volumen <= 100 && (ok = control->lpVtbl->SetMasterVolumeLevelScalar(control, volumen*0.01, NULL)) == S_OK){
              printf("Volumen establecido al: %.f porciento \n\n", volumen);
              resp = 0;
              bucle_function_volume(control);
            }break;
        case 2:
            if((ok = control->lpVtbl->GetMasterVolumeLevelScalar(control, &volumen)) == S_OK){
              printf("Volumen actual es: %.f \n\n", volumen*100);
              resp = 0;
              bucle_function_volume(control);
            }break;
        case 3:
            if((ok = control->lpVtbl->GetMute(control, &mute)) == S_OK){
              printf("Volumen : %s \n\n", (mute)? "esta en mute":"no esta en mute");
              resp = 0;
              bucle_function_volume(control);
            }break;
        case 4:
            if((ok = control->lpVtbl->GetMute(control, &mute)) == S_OK){
              ok = (mute)? control->lpVtbl->SetMute(control, 0, NULL): control->lpVtbl->SetMute(control, 1, NULL);
              if(ok == S_OK){
                  printf("Volumen : %s \n\n", (mute)? "se quito mute":"se ha puesto en mute");
                  resp = 0;
                  bucle_function_volume(control);
              }
            }break;
        default:
            resp = 0;
            break;
    }

return resp;
}

void bucle_function_volume(IAudioEndpointVolume* control){
    function_volume(control);
}
