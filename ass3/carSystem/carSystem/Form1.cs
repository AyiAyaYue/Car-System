using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace carSystem
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            serialPort1.Open();
            timer1.Enabled = true;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (serialPort1.BytesToRead > 0)
            {
                string message = serialPort1.ReadLine();
                message = message.Trim();
                if (message == "temp")
                {
                    string temp = serialPort1.ReadLine();
                    temp = temp.Trim();
                    tempLbl.Text = temp;
                } else if (message == "headlight")
                {
                    string headlight = serialPort1.ReadLine();
                    headlight = headlight.Trim();
                    headlightLbl.Text = headlight;
                } else if (message == "turn")
                {
                    string turn = serialPort1.ReadLine();
                    turn = turn.Trim();
                    info.Items.Add(turn);
                }
            }
        } 

        private void alarmBtn_Click(object sender, EventArgs e)
        {
            var alarmBtn = sender as Button;

            if (alarmBtn.BackColor == Color.White)
            {
                serialPort1.WriteLine("alarm");
                alarmBtn.BackColor = Color.Red;
            } else if (alarmBtn.BackColor == Color.Red) {
                serialPort1.WriteLine("noalarm");
                alarmBtn.BackColor = Color.White;
            }
            

           
        }
    }
}
