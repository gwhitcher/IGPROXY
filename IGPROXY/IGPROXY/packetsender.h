#pragma once
#include <string>
#include "igproxy.h"


namespace IGPROXY {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;


	/// <summary>
	/// Summary for packetsender
	/// </summary>
	public ref class packetsender : public System::Windows::Forms::Form
	{
	public:
		packetsender(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~packetsender()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TextBox^  packetdata;
	protected: 


	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::LinkLabel^  linkLabel1;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(packetsender::typeid));
			this->packetdata = (gcnew System::Windows::Forms::TextBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->linkLabel1 = (gcnew System::Windows::Forms::LinkLabel());
			this->SuspendLayout();
			// 
			// packetdata
			// 
			this->packetdata->Location = System::Drawing::Point(1, 1);
			this->packetdata->Name = L"packetdata";
			this->packetdata->Size = System::Drawing::Size(281, 20);
			this->packetdata->TabIndex = 4;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(206, 29);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Send";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &packetsender::button3_Click);
			// 
			// linkLabel1
			// 
			this->linkLabel1->ActiveLinkColor = System::Drawing::Color::Black;
			this->linkLabel1->AutoSize = true;
			this->linkLabel1->LinkColor = System::Drawing::Color::Black;
			this->linkLabel1->Location = System::Drawing::Point(1, 38);
			this->linkLabel1->Name = L"linkLabel1";
			this->linkLabel1->Size = System::Drawing::Size(104, 13);
			this->linkLabel1->TabIndex = 5;
			this->linkLabel1->TabStop = true;
			this->linkLabel1->Text = L"InfamousGamerz.net";
			this->linkLabel1->UseMnemonic = false;
			this->linkLabel1->VisitedLinkColor = System::Drawing::Color::Black;
			this->linkLabel1->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &packetsender::linkLabel1_LinkClicked);
			// 
			// packetsender
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 54);
			this->Controls->Add(this->linkLabel1);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->packetdata);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"packetsender";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Packet Sender";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

public: void MarshalString(String ^ s, string& os) {
			using namespace Runtime::InteropServices;
			const char* chars =
				(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
			os = chars;
			Marshal::FreeHGlobal(IntPtr((void*)chars));
}

private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			 String ^ packet_info = packetdata->Text;
			 if (packet_info == "") {
				MessageBox::Show("Packet data cannot be empty!", "Error");
			 } else {
				MessageBox::Show("Packet Sent: "+packet_info, "Success!");
				System::String^ original = packet_info;
				std::string container = "";
				MarshalString(original, container);
				send_packet(container);
			 }
		 }

private: System::Void linkLabel1_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
		 System::Diagnostics::Process::Start("http://infamousgamerz.net");
		 }
};
}
