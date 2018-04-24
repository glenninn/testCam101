#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <dshow.h>
#include <conio.h>


typedef struct my_struct_type my_short_type_t;

struct devInfoRec {
	BSTR name;
	IBaseFilter * pDeviceFilter;
	IAMCameraControl * pCameraControl;
};

#define MAX_DEVICES 8
devInfoRec  cameras[MAX_DEVICES];
int numCameras = 0;
int curCamera = 0;
int zoomSpan = 0;

void showField(IAMVideoProcAmp *pProcAmp, char * pName, long prop) {
	long Min, Max, Step, Default, Flags;

	HRESULT hr = pProcAmp->GetRange(prop, &Min, &Max, &Step, &Default, &Flags);
	if (SUCCEEDED(hr)) {
		printf("\n* %s [min..max]:  %d .. %d", pName, Min, Max);
	}
	else {
		printf("\n* %s: <error>: %x", pName, hr);
	}
}

//---------------------------------------------------------
// Direct show primitives to access Camera attributes
//
bool setZoom(IAMCameraControl *pCameraControl, long newZoom)
{
	HRESULT hr = pCameraControl->Set(CameraControl_Zoom, newZoom, 0L);
	return SUCCEEDED(hr);
}

bool getZoom(IAMCameraControl *pCameraControl, long * pZoom)
{
	long zFlags = 0;
	HRESULT hr = pCameraControl->Get(CameraControl_Zoom, pZoom, &zFlags);
	return SUCCEEDED(hr);
}

bool getPanRange(IAMCameraControl *pCameraControl, long *pMin, long *pMax)
{
	long Step, Default, Flags;
	HRESULT hr = pCameraControl->GetRange(CameraControl_Pan, pMin, pMax, &Step, &Default, &Flags);
	return SUCCEEDED(hr);
}

bool getPan(IAMCameraControl *pCameraControl, long * pPan)
{
	long zFlags = 0;
	HRESULT hr = pCameraControl->Get(CameraControl_Pan, pPan, &zFlags);
	return SUCCEEDED(hr);
}

bool setPan(IAMCameraControl *pCameraControl, long newPan)
{
	HRESULT hr = pCameraControl->Set(CameraControl_Pan, newPan, 0L);
	return SUCCEEDED(hr);
}

bool getTiltRange(IAMCameraControl *pCameraControl, long *pMin, long *pMax)
{
	long Step, Default, Flags;
	HRESULT hr = pCameraControl->GetRange(CameraControl_Tilt, pMin, pMax, &Step, &Default, &Flags);
	return SUCCEEDED(hr);
}

bool getTilt(IAMCameraControl *pCameraControl, long * pTilt)
{
	long zFlags = 0;
	HRESULT hr = pCameraControl->Get(CameraControl_Tilt, pTilt, &zFlags);
	return SUCCEEDED(hr);
}

bool setTilt(IAMCameraControl *pCameraControl, long newTilt)
{
	HRESULT hr = pCameraControl->Set(CameraControl_Tilt, newTilt, 0L);
	return SUCCEEDED(hr);
}

bool getZoomRange(IAMCameraControl *pCameraControl, long *pMin, long *pMax)
{
	long Step, Default, Flags;
	HRESULT hr = pCameraControl->GetRange(CameraControl_Zoom, pMin, pMax, &Step, &Default, &Flags);
	return SUCCEEDED(hr);
}

//-----------------------------------------------------------------
// UI functions to display/modify Camera Attributes
//
void showZoomInfo(IAMCameraControl *pCameraControl)
{
	long Min, Max, Val;
	printf("\n****************************************************");
	// Get the range and default values 
	if (getZoomRange(pCameraControl, &Min, &Max)
		&& getZoom(pCameraControl, &Val)
		) {
		// List out the zoom info
		printf("\n* Camera Zoom:           %d", Val);
		printf("\n* ZoomRange [min..max]:  %d..%d", Min, Max);
	}
	else {
		printf("\n!! Unable to Read Zoom info");
	}
}

void showPanInfo(IAMCameraControl *pCameraControl)
{
	long Min, Max, Val;
	printf("\n****************************************************");
	// Get the range and default values 
	if (getPanRange(pCameraControl, &Min, &Max)
		&& getPan(pCameraControl, &Val)
		) {
		// List out the zoom info
		printf("\n* Camera Pan:            %d", Val);
		printf("\n* Pan Range [min..max]:  %d..%d", Min, Max);
	}
	else {
		printf("\n!! Unable to Read Pan info");
	}
}

void showTiltInfo(IAMCameraControl *pCameraControl)
{
	long Min, Max, Val;
	printf("\n****************************************************");
	// Get the range and default values 
	if (getTiltRange(pCameraControl, &Min, &Max)
		&& getTilt(pCameraControl, &Val)
		) {
		// List out the zoom info
		printf("\n* Camera Tilt:           %d", Val);
		printf("\n* Tilt Range [min..max]: %d..%d", Min, Max);
	}
	else {
		printf("\n!! Unable to Read Tilt info");
	}
}

void showCameraInfo(int whichOne)
{
	showZoomInfo(cameras[whichOne].pCameraControl);
	showPanInfo(cameras[whichOne].pCameraControl);
	showTiltInfo(cameras[whichOne].pCameraControl);

	// Query the capture filter for the IAMVideoProcAmp interface.
	IAMVideoProcAmp *pProcAmp = 0;
	HRESULT hr = cameras[whichOne].pDeviceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
	if (FAILED(hr))
	{
		// The device does not support IAMVideoProcAmp
	}
	else
	{
		// Get the range and default values 
		showField(pProcAmp, "Brightness", VideoProcAmp_Brightness);
		showField(pProcAmp, "Backlight", VideoProcAmp_BacklightCompensation);
		showField(pProcAmp, "Contrast", VideoProcAmp_Contrast);
		showField(pProcAmp, "Saturation", VideoProcAmp_Saturation);
		showField(pProcAmp, "Sharpness", VideoProcAmp_Sharpness);
		showField(pProcAmp, "White Balance", VideoProcAmp_WhiteBalance);
/*
		if (SUCCEEDED(hr))
		{
			hr = pProcAmp->Set(VideoProcAmp_Brightness, 142, VideoProcAmp_Flags_Manual);
			hr = pProcAmp->Set(VideoProcAmp_BacklightCompensation, 0, VideoProcAmp_Flags_Manual);
			hr = pProcAmp->Set(VideoProcAmp_Contrast, 4, VideoProcAmp_Flags_Manual);
			hr = pProcAmp->Set(VideoProcAmp_Saturation, 100, VideoProcAmp_Flags_Manual);
			hr = pProcAmp->Set(VideoProcAmp_Sharpness, 0, VideoProcAmp_Flags_Manual);
			hr = pProcAmp->Set(VideoProcAmp_WhiteBalance, 2800, VideoProcAmp_Flags_Manual);
		}
*/
	}
}

void getCameraDevices()
{
	// for playing
	// to select a video input device
	ICreateDevEnum *pCreateDevEnum = NULL;
	IEnumMoniker *pEnumMoniker = NULL;
	IMoniker *pMoniker = NULL;
	ULONG nFetched = 0;

	//
	// selecting a device
	//

	// Create CreateDevEnum to list device
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (PVOID *)&pCreateDevEnum);

	// Create EnumMoniker to list VideoInputDevice 
	pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
		&pEnumMoniker, 0);
	if (pEnumMoniker == NULL) {
		// this will be shown if there is no capture device
		printf("\n!! no devices\n");
		return;
	}


	ICaptureGraphBuilder2 *pCaptureGraphBuilder2;

	// create CaptureGraphBuilder2
	CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2,
		(LPVOID *)&pCaptureGraphBuilder2);

	//============================================================
	//===========  MY CODE  ======================================
	// https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/4c34b98e-0af9-41e6-8d2d-7f15318f89ee/sample-code-to-control-usb-webcam-and-adjust-its-settings?forum=windowsdirectshowdevelopment
	//=============================================================
	HRESULT hr = CoInitialize(0);

	// reset EnumMoniker
	pEnumMoniker->Reset();

	// get each Moniker
	while (pEnumMoniker->Next(1, &pMoniker, &nFetched) == S_OK) {
		IPropertyBag *pPropertyBag;

		// bind to IPropertyBag
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
			(void **)&pPropertyBag);

		if (numCameras < MAX_DEVICES) {
			VARIANT var;
			VariantInit(&var);

			// get FriendlyName
			pPropertyBag->Read(L"FriendlyName", &var, 0);
			cameras[numCameras].name = ::SysAllocString(var.bstrVal);
			VariantClear(&var);
			pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&cameras[numCameras].pDeviceFilter);

			IAMStreamConfig *pConfig = NULL;
			HRESULT hrdf = pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, 0, 
				                cameras[numCameras].pDeviceFilter,IID_IAMStreamConfig, (void**)&pConfig);

			int iCount = 0, iSize = 0;
			hrdf = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

			// Query the capture filter for the IAMCameraControl interface.
			IAMCameraControl *pCameraControl = 0;
			hrdf = cameras[numCameras].pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void**)&cameras[numCameras].pCameraControl);


			numCameras++;
			// release
			pMoniker->Release();
		}
		pPropertyBag->Release();
	}

	// release
	pCaptureGraphBuilder2->Release();
	pEnumMoniker->Release();
	pCreateDevEnum->Release();

}

void listCameraDevices()
{
	printf("\n\n****** [ %d ] Cameras Detected ********", numCameras);
	for (int i = 0; i < numCameras; i++) {
		printf("\n%s", (i==curCamera) ? "-->" : "   ");
		printf("%d) %S   --> %X", i, cameras[i].name, cameras[i].pDeviceFilter);
	}
	printf("\n");
}

void setZoomOnACamera()
{
	char buf[64];
	long curZoom = 0;
	long minZ, maxZ;

	printf("\n**** Set Zoom ***");
	showZoomInfo(cameras[curCamera].pCameraControl);

	if (!getZoomRange(cameras[curCamera].pCameraControl, &minZ, &maxZ)) {
		printf("\n!! Error reading camera zoom range");
		return;
	}

	if (!getZoom(cameras[curCamera].pCameraControl, &curZoom)) {
		printf("\n!! Error accessing camera's current zoom value");
		return;
	}

	printf("\nEnter New Zoom value Device [current: %d] > ", curZoom);
	scanf("%s", buf);
	int n = sscanf(buf, "%d", &curZoom);
	if (n == 1) {

		if ((curZoom < minZ) || (curZoom > maxZ)) {
			printf("\n? invalid zoom setting");
		}
		else {
			if (setZoom(cameras[curCamera].pCameraControl, curZoom)) {
				printf("\n-->  (%S) Zoom set!", cameras[curCamera].name);
			} else printf("\n? Error setting zoom");
		}
	}
	else {
		printf("? unable to read zoom setting");
	}
}

void setCamera(int whichOne)
{
	long zMin, zMax;
	curCamera = whichOne;
	HRESULT hr = getZoomRange(cameras[curCamera].pCameraControl, &zMin, &zMax);
	if (SUCCEEDED(hr))
		zoomSpan = zMax - zMin;
	showCameraInfo(curCamera);
}

void selectCamera()
{
	int whichCamera = 0;
	char buf[256];
	listCameraDevices();
	printf("\nSelect Camera [current: %d] > ",whichCamera);
	scanf("%s", buf);
	int n = sscanf(buf,"%d", &whichCamera);
	if (n == 1) {
		if ((whichCamera >= 0) && (whichCamera < numCameras)) {
			setCamera(whichCamera);
			printf("\n-->  Selecting camera: %d (%S)", curCamera, cameras[curCamera].name);
		}
		else {
			printf("? invalid camera number");
		}
	}
	else {
		printf("? unable to read camera number %d",whichCamera);
	}
	printf("\n");
}

void panLR(int dt) 
{
	int delta = dt * 1;
	long curP = 0;

	if (!getPan(cameras[curCamera].pCameraControl, &curP)) {
		printf("\n!! Error accessing camera's current pan value");
		return;
	}

	curP += delta;
	if (setPan(cameras[curCamera].pCameraControl, curP)) {
		printf("\n-->  (%S) Pan set!", cameras[curCamera].name);
	}
	else printf("\n? Error setting camera's panning");
}

void panUD(int dt)
{
	int delta = dt * 1;
	long curT = 0;

	if (!getTilt(cameras[curCamera].pCameraControl, &curT)) {
		printf("\n!! Error accessing camera's current tilt value");
		return;
	}
	curT += delta;
	if (setTilt(cameras[curCamera].pCameraControl, curT)) {
		printf("\n-->  (%S) Tilt set!", cameras[curCamera].name);
	}
	else printf("\n? Error setting camera's tilt");
}

void zoomIn(int dt)
{
	int delta = dt * zoomSpan / 10;
	long curZ = 0;

	if (!getZoom(cameras[curCamera].pCameraControl, &curZ)) {
		printf("\n!! Error accessing camera's current zoom value");
		return;
	}
	curZ += delta;
	if (setZoom(cameras[curCamera].pCameraControl, curZ)) {
		printf("\n-->  (%S) Zoom set!", cameras[curCamera].name);
	}
	else printf("\n? Error setting camera's zoom");
}

void showKeymap()
{
	printf("\n      +-------+-------+-------+-------+");
	printf("\n      | 7 ___ | 8 up  | 9 ___ | - Zout|");
	printf("\n      +-------+-------+-------+-------+");
	printf("\n      | 4 Left| 5 ___ | 6 Rght| + Zin |");
	printf("\n      +-------+-------+-------+-------+");
	printf("\n      | 1 ___ | 2 dn  | 3 ___ |");
	printf("\n      +-------+-------+-------+");
}

void processCommands()
{
	char c;
	bool done = false;

	do {
		printf("\n\n l: list,  z: zoom,  i: info,  s: select cam,  \n '?' : help  q: quit >> ");
		int kp;
		do {
			kp = kbhit();
		} while (!kp);

		c = getch();
		switch (c) {
		case 'l': listCameraDevices();
			break;
		case 'z': setZoomOnACamera();
			break;
		case 's': selectCamera();
			break;
		case 'i': showCameraInfo(curCamera);
			break;
		case 'q': done = true;
			break;
		case '4':
		case '6': panLR((c == '4' ? -1 : 1));
			break;
		case '2':
		case '8': panUD((c == '2' ? -1 : 1));
			break;
		case '+':
		case '-': zoomIn((c == '+' ? 1 : -1));
			break;
		case '?': showKeymap();
			break;
		default: printf("\n? Unrecognized command: %c", c);
			break;
		}
	} while (!done);
}

void cameraCli()
{
	// initialize COM
	CoInitialize(NULL);

	getCameraDevices();
	listCameraDevices();
	showKeymap();
	processCommands();

	// need to de-alloc the memory for the BSTR's
	for (int i = 0; i < numCameras; i++) {
		::SysFreeString(cameras[i].name);
	}

	// finalize COM
	CoUninitialize();

}