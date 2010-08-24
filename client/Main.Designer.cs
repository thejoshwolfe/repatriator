namespace repatriator_client
{
    partial class Main
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.connectButton = new System.Windows.Forms.Button();
            this.hostNameText = new System.Windows.Forms.TextBox();
            this.hostPortText = new System.Windows.Forms.TextBox();
            this.liveViewPictureBox = new System.Windows.Forms.PictureBox();
            this.takePictureButton = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            this.downlaodAllButton = new System.Windows.Forms.Button();
            this.liftSliderZ = new System.Windows.Forms.Panel();
            this.lifeSliderZ = new System.Windows.Forms.TrackBar();
            this.miniMapPictureBox = new System.Windows.Forms.PictureBox();
            this.butterflyElemtnHost = new System.Windows.Forms.Integration.ElementHost();
            this.butterflySliderX = new System.Windows.Forms.TrackBar();
            this.butterflySliderY = new System.Windows.Forms.TrackBar();
            this.miniMapSliderX = new System.Windows.Forms.TrackBar();
            this.miniMapSliderY = new System.Windows.Forms.TrackBar();
            this.panel3 = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).BeginInit();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.liftSliderZ.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.lifeSliderZ)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderY)).BeginInit();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(204, 14);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(76, 23);
            this.connectButton.TabIndex = 2;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // hostNameText
            // 
            this.hostNameText.Location = new System.Drawing.Point(12, 14);
            this.hostNameText.Name = "hostNameText";
            this.hostNameText.Size = new System.Drawing.Size(140, 20);
            this.hostNameText.TabIndex = 3;
            this.hostNameText.Text = "localhost";
            // 
            // hostPortText
            // 
            this.hostPortText.Location = new System.Drawing.Point(158, 14);
            this.hostPortText.Name = "hostPortText";
            this.hostPortText.Size = new System.Drawing.Size(40, 20);
            this.hostPortText.TabIndex = 7;
            this.hostPortText.Text = "9999";
            // 
            // liveViewPictureBox
            // 
            this.liveViewPictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.liveViewPictureBox.Location = new System.Drawing.Point(0, 0);
            this.liveViewPictureBox.Name = "liveViewPictureBox";
            this.liveViewPictureBox.Size = new System.Drawing.Size(427, 363);
            this.liveViewPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.liveViewPictureBox.TabIndex = 8;
            this.liveViewPictureBox.TabStop = false;
            // 
            // takePictureButton
            // 
            this.takePictureButton.Location = new System.Drawing.Point(34, 201);
            this.takePictureButton.Name = "takePictureButton";
            this.takePictureButton.Size = new System.Drawing.Size(71, 156);
            this.takePictureButton.TabIndex = 9;
            this.takePictureButton.Text = "Take Picture";
            this.takePictureButton.UseVisualStyleBackColor = true;
            this.takePictureButton.Click += new System.EventHandler(this.takePictureButton_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Controls.Add(this.downlaodAllButton);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 363);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(544, 100);
            this.panel1.TabIndex = 10;
            // 
            // panel2
            // 
            this.panel2.AutoScroll = true;
            this.panel2.BackColor = System.Drawing.Color.Maroon;
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.hostNameText);
            this.panel2.Controls.Add(this.connectButton);
            this.panel2.Controls.Add(this.hostPortText);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(427, 100);
            this.panel2.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(404, 30);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "label1";
            // 
            // downlaodAllButton
            // 
            this.downlaodAllButton.Dock = System.Windows.Forms.DockStyle.Right;
            this.downlaodAllButton.Location = new System.Drawing.Point(427, 0);
            this.downlaodAllButton.Name = "downlaodAllButton";
            this.downlaodAllButton.Size = new System.Drawing.Size(117, 100);
            this.downlaodAllButton.TabIndex = 0;
            this.downlaodAllButton.Text = "Download All";
            this.downlaodAllButton.UseVisualStyleBackColor = true;
            // 
            // liftSliderZ
            // 
            this.liftSliderZ.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(255)))));
            this.liftSliderZ.Controls.Add(this.takePictureButton);
            this.liftSliderZ.Controls.Add(this.lifeSliderZ);
            this.liftSliderZ.Controls.Add(this.miniMapPictureBox);
            this.liftSliderZ.Controls.Add(this.butterflyElemtnHost);
            this.liftSliderZ.Controls.Add(this.butterflySliderX);
            this.liftSliderZ.Controls.Add(this.butterflySliderY);
            this.liftSliderZ.Controls.Add(this.miniMapSliderX);
            this.liftSliderZ.Controls.Add(this.miniMapSliderY);
            this.liftSliderZ.Dock = System.Windows.Forms.DockStyle.Right;
            this.liftSliderZ.Location = new System.Drawing.Point(427, 0);
            this.liftSliderZ.Name = "liftSliderZ";
            this.liftSliderZ.Size = new System.Drawing.Size(117, 363);
            this.liftSliderZ.TabIndex = 11;
            // 
            // lifeSliderZ
            // 
            this.lifeSliderZ.LargeChange = 0;
            this.lifeSliderZ.Location = new System.Drawing.Point(3, 201);
            this.lifeSliderZ.Name = "lifeSliderZ";
            this.lifeSliderZ.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.lifeSliderZ.Size = new System.Drawing.Size(45, 156);
            this.lifeSliderZ.TabIndex = 7;
            // 
            // miniMapPictureBox
            // 
            this.miniMapPictureBox.Location = new System.Drawing.Point(26, 28);
            this.miniMapPictureBox.Name = "miniMapPictureBox";
            this.miniMapPictureBox.Size = new System.Drawing.Size(79, 68);
            this.miniMapPictureBox.TabIndex = 6;
            this.miniMapPictureBox.TabStop = false;
            // 
            // butterflyElemtnHost
            // 
            this.butterflyElemtnHost.Location = new System.Drawing.Point(26, 127);
            this.butterflyElemtnHost.Name = "butterflyElemtnHost";
            this.butterflyElemtnHost.Size = new System.Drawing.Size(79, 68);
            this.butterflyElemtnHost.TabIndex = 3;
            this.butterflyElemtnHost.Text = "elementHost1";
            this.butterflyElemtnHost.Child = null;
            // 
            // butterflySliderX
            // 
            this.butterflySliderX.LargeChange = 0;
            this.butterflySliderX.Location = new System.Drawing.Point(26, 102);
            this.butterflySliderX.Name = "butterflySliderX";
            this.butterflySliderX.Size = new System.Drawing.Size(79, 45);
            this.butterflySliderX.TabIndex = 5;
            this.butterflySliderX.TickStyle = System.Windows.Forms.TickStyle.None;
            // 
            // butterflySliderY
            // 
            this.butterflySliderY.LargeChange = 0;
            this.butterflySliderY.Location = new System.Drawing.Point(3, 127);
            this.butterflySliderY.Name = "butterflySliderY";
            this.butterflySliderY.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.butterflySliderY.Size = new System.Drawing.Size(45, 68);
            this.butterflySliderY.TabIndex = 4;
            this.butterflySliderY.TickStyle = System.Windows.Forms.TickStyle.None;
            // 
            // miniMapSliderX
            // 
            this.miniMapSliderX.LargeChange = 0;
            this.miniMapSliderX.Location = new System.Drawing.Point(26, 3);
            this.miniMapSliderX.Name = "miniMapSliderX";
            this.miniMapSliderX.Size = new System.Drawing.Size(79, 45);
            this.miniMapSliderX.TabIndex = 2;
            this.miniMapSliderX.TickStyle = System.Windows.Forms.TickStyle.None;
            // 
            // miniMapSliderY
            // 
            this.miniMapSliderY.LargeChange = 0;
            this.miniMapSliderY.Location = new System.Drawing.Point(3, 28);
            this.miniMapSliderY.Name = "miniMapSliderY";
            this.miniMapSliderY.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.miniMapSliderY.Size = new System.Drawing.Size(45, 68);
            this.miniMapSliderY.TabIndex = 1;
            this.miniMapSliderY.TickStyle = System.Windows.Forms.TickStyle.None;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.liveViewPictureBox);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Location = new System.Drawing.Point(0, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(427, 363);
            this.panel3.TabIndex = 12;
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 463);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.liftSliderZ);
            this.Controls.Add(this.panel1);
            this.Name = "Main";
            this.Text = "Repatriator";
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.liftSliderZ.ResumeLayout(false);
            this.liftSliderZ.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.lifeSliderZ)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderY)).EndInit();
            this.panel3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.TextBox hostNameText;
        private System.Windows.Forms.TextBox hostPortText;
        private System.Windows.Forms.PictureBox liveViewPictureBox;
        private System.Windows.Forms.Button takePictureButton;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button downlaodAllButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel liftSliderZ;
        private System.Windows.Forms.TrackBar miniMapSliderY;
        private System.Windows.Forms.TrackBar miniMapSliderX;
        private System.Windows.Forms.Integration.ElementHost butterflyElemtnHost;
        private System.Windows.Forms.TrackBar butterflySliderX;
        private System.Windows.Forms.TrackBar butterflySliderY;
        private System.Windows.Forms.PictureBox miniMapPictureBox;
        private System.Windows.Forms.TrackBar lifeSliderZ;
        private System.Windows.Forms.Panel panel3;

    }
}