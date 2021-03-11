using System;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Threading;
using System.ComponentModel;
using System.Windows.Forms;

namespace PasmIDE.Service
{
    enum EIdeApiMessageType
    {
        EIAMT_INFO,
        EIAMT_LEXICAL_ERROR,
        EIAMT_SYNTACTIC_ERROR,
        EIAMT_RUNTIME_ERROR
    }

    [StructLayout(LayoutKind.Sequential)]
    struct IdeMessage
    {
        public int dwType;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string szMessage;
    }

    class ThreadArg
    {
        public EventWaitHandle hRecvEvent
        {
            get;
            set;
        }

        public Mutex hSendEvent
        {
            get;
            set;
        }

        public MemoryMappedFile hSharedMemory
        {
            get;
            set;
        }

        public ListBox listBox
        {
            get;
            set;
        }

        public ThreadArg() { }
    }

    class IdeService
    {
        static string HandleIdeMessage(IdeMessage ideMessage)
        {
            string result = null;

            EIdeApiMessageType eType = (EIdeApiMessageType)ideMessage.dwType;
            switch (eType)
            {
                case EIdeApiMessageType.EIAMT_INFO:
                    result += "Информация: ";
                    break;
                case EIdeApiMessageType.EIAMT_LEXICAL_ERROR:
                    result += "Лексическая ошибка: ";
                    break;
                case EIdeApiMessageType.EIAMT_SYNTACTIC_ERROR:
                    result += "Синтаксическая ошибка: ";
                    break;
                case EIdeApiMessageType.EIAMT_RUNTIME_ERROR:
                    result += "Ошибка времени исполнения: ";
                    break;
                default:
                    return null;
            }

            result += ideMessage.szMessage;

            return result;
        }

        static readonly string SHARED_MEMORY_NAME = "PasmIdeSharedMemory";
        static readonly string RECV_EVENT_NAME = "PasmIdeRecvEvent";
        static readonly string SEND_EVENT_NAME = "PasmIdeSendEvent";

        private MemoryMappedFile hSharedMemory;
        private EventWaitHandle hRecvEvent;
        private Mutex hSendEvent;
        private BackgroundWorker hThread;

        public IdeService()
        {
            
        }

        public void Initialize(ListBox listBox)
        {
            int size = Marshal.SizeOf(typeof(IdeMessage));
            hSharedMemory = MemoryMappedFile.CreateNew(SHARED_MEMORY_NAME, size);
            hRecvEvent = new EventWaitHandle(false, EventResetMode.AutoReset, RECV_EVENT_NAME);
            hSendEvent = new Mutex(false, SEND_EVENT_NAME);

            ThreadArg arg = new ThreadArg();
            arg.hSendEvent = hSendEvent;
            arg.hRecvEvent = hRecvEvent;
            arg.hSharedMemory = hSharedMemory;
            arg.listBox = listBox;


            hThread = new BackgroundWorker();
            hThread.WorkerSupportsCancellation = true;

            hThread.DoWork += HThread_DoWork;
            hThread.RunWorkerAsync(arg);
        }

        private void HThread_DoWork(object sender, DoWorkEventArgs e)
        {
            ThreadArg context = (ThreadArg)e.Argument;

           
            while (!hThread.CancellationPending)
            {
                try
                {
                    WaitHandle.WaitAny(new WaitHandle[] { context.hRecvEvent });

                    int size = Marshal.SizeOf(typeof(IdeMessage));
                    using (var accessor = context.hSharedMemory.CreateViewAccessor())
                    {
                        IdeMessage ideMessage;

                        byte[] data = new byte[size];
                        accessor.ReadArray(0, data, 0, size);

                        IntPtr p = Marshal.AllocHGlobal(size);
                        Marshal.Copy(data, 0, p, size);

                        ideMessage = (IdeMessage)Marshal.PtrToStructure(p, typeof(IdeMessage));

                        Marshal.FreeHGlobal(p);

                        string message = HandleIdeMessage(ideMessage);
                        if (message == null)
                        {
                            continue;
                        }

                        context.listBox.Invoke((MethodInvoker)delegate
                        {
                            context.listBox.Items.Add(message);
                        });

                        context.hSendEvent.ReleaseMutex();
                    }
                }
                catch
                {
                    context.hSendEvent.ReleaseMutex();
                    return;
                }
            }
        }

        public void Deinitialize()
        {
            hSharedMemory.Dispose();
            hRecvEvent.Dispose();

            hThread.CancelAsync();
        }
    }
}
