namespace repatriator_client
{
    partial class MainWindow
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
            System.Windows.Forms.Label label1;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label5;
            this.connectButton = new System.Windows.Forms.Button();
            this.serverText = new System.Windows.Forms.TextBox();
            this.liveViewPictureBox = new System.Windows.Forms.PictureBox();
            this.takePictureButton = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.downlaodAllButton = new System.Windows.Forms.Button();
            this.liftSliderZ = new System.Windows.Forms.Panel();
            this.butterflyElemtnHost = new System.Windows.Forms.Integration.ElementHost();
            this.lifeSliderZ = new System.Windows.Forms.TrackBar();
            this.miniMapPictureBox = new System.Windows.Forms.PictureBox();
            this.butterflySliderX = new System.Windows.Forms.TrackBar();
            this.butterflySliderY = new System.Windows.Forms.TrackBar();
            this.miniMapSliderX = new System.Windows.Forms.TrackBar();
            this.miniMapSliderY = new System.Windows.Forms.TrackBar();
            this.panel3 = new System.Windows.Forms.Panel();
            this.setupPanel = new System.Windows.Forms.Panel();
            this.downloadDirectoryText = new System.Windows.Forms.TextBox();
            this.passwordText = new System.Windows.Forms.TextBox();
            this.cancelConncetionButton = new System.Windows.Forms.Button();
            this.statusLabel = new System.Windows.Forms.Label();
            this.userNameText = new System.Windows.Forms.TextBox();
            label1 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            label5 = new System.Windows.Forms.Label();
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
            this.setupPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(406, 33);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(35, 13);
            label1.TabIndex = 0;
            label1.Text = "label1";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(40, 79);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(39, 13);
            label2.TabIndex = 9;
            label2.Text = "server:";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(20, 105);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(59, 13);
            label3.TabIndex = 10;
            label3.Text = "user name:";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(24, 131);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(55, 13);
            label4.TabIndex = 14;
            label4.Text = "password:";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new System.Drawing.Point(9, 157);
            label5.Name = "label5";
            label5.Size = new System.Drawing.Size(70, 13);
            label5.TabIndex = 16;
            label5.Text = "download dir:";
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(85, 201);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(76, 23);
            this.connectButton.TabIndex = 2;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // serverText
            // 
            this.serverText.Location = new System.Drawing.Point(85, 76);
            this.serverText.Name = "serverText";
            this.serverText.Size = new System.Drawing.Size(157, 20);
            this.serverText.TabIndex = 3;
            this.serverText.Text = "localhost:57051";
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
            this.panel2.Controls.Add(label1);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(427, 100);
            this.panel2.TabIndex = 1;
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
            this.liftSliderZ.Controls.Add(this.butterflyElemtnHost);
            this.liftSliderZ.Controls.Add(this.takePictureButton);
            this.liftSliderZ.Controls.Add(this.lifeSliderZ);
            this.liftSliderZ.Controls.Add(this.miniMapPictureBox);
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
            // butterflyElemtnHost
            // 
            this.butterflyElemtnHost.Location = new System.Drawing.Point(26, 127);
            this.butterflyElemtnHost.Name = "butterflyElemtnHost";
            this.butterflyElemtnHost.Size = new System.Drawing.Size(79, 68);
            this.butterflyElemtnHost.TabIndex = 3;
            this.butterflyElemtnHost.Text = "elementHost1";
            this.butterflyElemtnHost.Child = null;
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
            // butterflySliderX
            // 
            this.butterflySliderX.LargeChange = 0;
            this.butterflySliderX.Location = new System.Drawing.Point(26, 102);
            this.butterflySliderX.Maximum = 360;
            this.butterflySliderX.Name = "butterflySliderX";
            this.butterflySliderX.Size = new System.Drawing.Size(79, 45);
            this.butterflySliderX.TabIndex = 5;
            this.butterflySliderX.TickStyle = System.Windows.Forms.TickStyle.None;
            this.butterflySliderX.Value = 180;
            // 
            // butterflySliderY
            // 
            this.butterflySliderY.LargeChange = 0;
            this.butterflySliderY.Location = new System.Drawing.Point(3, 127);
            this.butterflySliderY.Maximum = 360;
            this.butterflySliderY.Name = "butterflySliderY";
            this.butterflySliderY.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.butterflySliderY.Size = new System.Drawing.Size(45, 68);
            this.butterflySliderY.TabIndex = 4;
            this.butterflySliderY.TickStyle = System.Windows.Forms.TickStyle.None;
            this.butterflySliderY.Value = 180;
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
            this.panel3.Controls.Add(this.setupPanel);
            this.panel3.Controls.Add(this.liveViewPictureBox);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Location = new System.Drawing.Point(0, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(427, 363);
            this.panel3.TabIndex = 12;
            // 
            // setupPanel
            // 
            this.setupPanel.Controls.Add(label5);
            this.setupPanel.Controls.Add(this.downloadDirectoryText);
            this.setupPanel.Controls.Add(label4);
            this.setupPanel.Controls.Add(this.passwordText);
            this.setupPanel.Controls.Add(this.cancelConncetionButton);
            this.setupPanel.Controls.Add(this.statusLabel);
            this.setupPanel.Controls.Add(label3);
            this.setupPanel.Controls.Add(label2);
            this.setupPanel.Controls.Add(this.userNameText);
            this.setupPanel.Controls.Add(this.serverText);
            this.setupPanel.Controls.Add(this.connectButton);
            this.setupPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.setupPanel.Location = new System.Drawing.Point(0, 0);
            this.setupPanel.Name = "setupPanel";
            this.setupPanel.Size = new System.Drawing.Size(427, 363);
            this.setupPanel.TabIndex = 9;
            // 
            // downloadDirectoryText
            // 
            this.downloadDirectoryText.Location = new System.Drawing.Point(85, 154);
            this.downloadDirectoryText.Name = "downloadDirectoryText";
            this.downloadDirectoryText.Size = new System.Drawing.Size(157, 20);
            this.downloadDirectoryText.TabIndex = 15;
            this.downloadDirectoryText.Text = "C:\\TEMP";
            // 
            // passwordText
            // 
            this.passwordText.Location = new System.Drawing.Point(85, 128);
            this.passwordText.Name = "passwordText";
            this.passwordText.Size = new System.Drawing.Size(157, 20);
            this.passwordText.TabIndex = 13;
            this.passwordText.Text = "temp1234";
            this.passwordText.UseSystemPasswordChar = true;
            // 
            // cancelConncetionButton
            // 
            this.cancelConncetionButton.Location = new System.Drawing.Point(167, 201);
            this.cancelConncetionButton.Name = "cancelConncetionButton";
            this.cancelConncetionButton.Size = new System.Drawing.Size(75, 23);
            this.cancelConncetionButton.TabIndex = 12;
            this.cancelConncetionButton.Text = "Cancel";
            this.cancelConncetionButton.UseVisualStyleBackColor = true;
            this.cancelConncetionButton.Click += new System.EventHandler(this.cancelConncetionButton_Click);
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(19, 245);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(60, 13);
            this.statusLabel.TabIndex = 11;
            this.statusLabel.Text = "status label";
            // 
            // userNameText
            // 
            this.userNameText.Location = new System.Drawing.Point(85, 102);
            this.userNameText.Name = "userNameText";
            this.userNameText.Size = new System.Drawing.Size(157, 20);
            this.userNameText.TabIndex = 8;
            this.userNameText.Text = "default_admin";
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 463);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.liftSliderZ);
            this.Controls.Add(this.panel1);
            this.Name = "MainWindow";
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
            this.setupPanel.ResumeLayout(false);
            this.setupPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.TextBox serverText;
        private System.Windows.Forms.PictureBox liveViewPictureBox;
        private System.Windows.Forms.Button takePictureButton;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button downlaodAllButton;
        private System.Windows.Forms.Panel liftSliderZ;
        private System.Windows.Forms.TrackBar miniMapSliderY;
        private System.Windows.Forms.TrackBar miniMapSliderX;
        private System.Windows.Forms.Integration.ElementHost butterflyElemtnHost;
        private System.Windows.Forms.TrackBar butterflySliderX;
        private System.Windows.Forms.TrackBar butterflySliderY;
        private System.Windows.Forms.PictureBox miniMapPictureBox;
        private System.Windows.Forms.TrackBar lifeSliderZ;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel setupPanel;
        private System.Windows.Forms.TextBox userNameText;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Button cancelConncetionButton;
        private System.Windows.Forms.TextBox passwordText;
        private System.Windows.Forms.TextBox downloadDirectoryText;

    }
}