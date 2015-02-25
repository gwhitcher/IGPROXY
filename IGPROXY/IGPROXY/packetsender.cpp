#include <cstdlib>
#include <iostream>
#include "packetsender.h"
extern void send_packet();


using namespace std;
using namespace IGPROXY;

using namespace System;
using namespace System ::Windows::Forms;

[STAThread]
void packetsender_main() {
 Application::EnableVisualStyles();
 Application::SetCompatibleTextRenderingDefault(false);

 packetsender^ form = gcnew packetsender();
 Application::Run(form);
}