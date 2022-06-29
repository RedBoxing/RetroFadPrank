namespace RetroFadPrank
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            if (Program.BM != null)
            {
                e.Graphics.DrawImage(Program.BM, 0, 0);
            }
        }
    }
}