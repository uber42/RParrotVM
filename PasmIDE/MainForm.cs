using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using PasmIDE.Helpers;
using PasmIDE.Service;

namespace PasmIDE
{
    public partial class MainForm : Form
    {
        private string sCurrentFilePath = @"C:\Users\uber42\source\repos\Parrot\Compiler\Examples\hashtable.pasm";

        private string sCompilerPath = @"C:\Users\uber42\source\repos\Parrot\Debug\Compiler.exe";
        private string sVirtualMachinePath;

        private IniFile settings;

        private IdeService service;

        public MainForm()
        {
            InitializeComponent();

            settings = new IniFile("./Settings.ini");

            codeOpenDialog.Filter = "pasm files (*.pasm)|*.pasm|All files (*.*)|*.*";
            codeOpenDialog.RestoreDirectory = true;

            exeFileDialog.Filter = "Exe files (*.exe)|*.exe|All files (*.*)|*.*";
            exeFileDialog.RestoreDirectory = true;

            service = new IdeService();
            service.Initialize(messageList);
        }

        private void UpperBorder_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                FormDragLogic.Move(Handle);
            }
        }

        private void OpenDocument_Click(object sender, EventArgs e)
        {
            if (codeOpenDialog.ShowDialog() == DialogResult.OK)
            {
                sCurrentFilePath = codeOpenDialog.FileName;

                var fileStream = codeOpenDialog.OpenFile();

                using (StreamReader reader = new StreamReader(fileStream))
                {
                    codeBox.Text = reader.ReadToEnd();
                }
            }
        }

        private void SaveFile_Click(object sender, EventArgs e)
        {
            using (StreamWriter writer = new StreamWriter(sCurrentFilePath))
            {
                writer.Write(codeBox.Text);
            }
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void CompilerPath_Click(object sender, EventArgs e)
        {
            if (exeFileDialog.ShowDialog() == DialogResult.OK)
            {
                sCompilerPath = exeFileDialog.FileName;
            }
        }

        private void VmPath_Click(object sender, EventArgs e)
        {
            if (codeOpenDialog.ShowDialog() == DialogResult.OK)
            {
                sVirtualMachinePath = exeFileDialog.FileName;
            }
        }

        private void CompileButton_Click(object sender, EventArgs e)
        {
            Process process = new Process
            {
                StartInfo = new System.Diagnostics.ProcessStartInfo
                {
                    FileName = sCompilerPath,
                    Arguments = $"\"{sCurrentFilePath}\" -IDE",
                    UseShellExecute = false,
                    WindowStyle = System.Diagnostics.ProcessWindowStyle.Normal
                },
                EnableRaisingEvents = true
            };
            process.Start();

            process.WaitForExit();

            int code = process.ExitCode;
        }
    }
}
