using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using System.Windows.Interop; //HwndHost
using OpenGLCpp;

namespace OpenGLinWPF
{
    /// <summary>
    /// Interaction logic for OpenGLHWND.xaml
    /// </summary>
    public partial class OpenGLHWND : Window
    {
        public OpenGLHWND()
        {
            InitializeComponent();
        }

        private System.Windows.Threading.DispatcherTimer updateTimer = new System.Windows.Threading.DispatcherTimer();

        public override void BeginInit()
        {
            updateTimer.Interval = new TimeSpan(160000);
            updateTimer.Tick += new EventHandler(updateTimer_Tick);
            updateTimer.Start();
            base.BeginInit();
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            base.OnClosing(e);

            if (!e.Cancel)
            {
                if (null != updateTimer)
                {
                    updateTimer.Stop();
                    updateTimer = null;
                }
            }
        }

        private void updateTimer_Tick(object sender, EventArgs e)
        {
            if (null != hwndPlaceholder &&
                null != hwndPlaceholder.Child)
            {
                hwndPlaceholder.Child.InvalidateVisual();
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Create our OpenGL Hwnd 'control'...
            HwndHost host = new OpenGLCpp.OpenGLHwnd(); 

            // ... and attach it to the placeholder control:
            hwndPlaceholder.Child = host;
        }

    }
}
