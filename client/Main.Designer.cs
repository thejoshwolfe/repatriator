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
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.takePictureButton = new System.Windows.Forms.Button();
            this.elementHost1 = new System.Windows.Forms.Integration.ElementHost();
            this.butterflyControl1 = new repatriator_client.ButterflyControl();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(204, 12);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(76, 23);
            this.connectButton.TabIndex = 2;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // hostNameText
            // 
            this.hostNameText.Location = new System.Drawing.Point(12, 12);
            this.hostNameText.Name = "hostNameText";
            this.hostNameText.Size = new System.Drawing.Size(140, 20);
            this.hostNameText.TabIndex = 3;
            this.hostNameText.Text = "localhost";
            // 
            // hostPortText
            // 
            this.hostPortText.Location = new System.Drawing.Point(158, 12);
            this.hostPortText.Name = "hostPortText";
            this.hostPortText.Size = new System.Drawing.Size(40, 20);
            this.hostPortText.TabIndex = 7;
            this.hostPortText.Text = "9999";
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(12, 70);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(268, 192);
            this.pictureBox.TabIndex = 8;
            this.pictureBox.TabStop = false;
            // 
            // takePictureButton
            // 
            this.takePictureButton.Location = new System.Drawing.Point(204, 41);
            this.takePictureButton.Name = "takePictureButton";
            this.takePictureButton.Size = new System.Drawing.Size(76, 23);
            this.takePictureButton.TabIndex = 9;
            this.takePictureButton.Text = "Take Picture";
            this.takePictureButton.UseVisualStyleBackColor = true;
            this.takePictureButton.Click += new System.EventHandler(this.takePictureButton_Click);
            // 
            // elementHost1
            // 
            this.elementHost1.Location = new System.Drawing.Point(286, 12);
            this.elementHost1.Name = "elementHost1";
            this.elementHost1.Size = new System.Drawing.Size(220, 250);
            this.elementHost1.TabIndex = 10;
            this.elementHost1.Text = "elementHost1";
            this.elementHost1.Child = this.butterflyControl1;
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(518, 276);
            this.Controls.Add(this.elementHost1);
            this.Controls.Add(this.takePictureButton);
            this.Controls.Add(this.pictureBox);
            this.Controls.Add(this.hostPortText);
            this.Controls.Add(this.hostNameText);
            this.Controls.Add(this.connectButton);
            this.Name = "Main";
            this.Text = "Repatriator";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.TextBox hostNameText;
        private System.Windows.Forms.TextBox hostPortText;
        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.Button takePictureButton;
        private System.Windows.Forms.Integration.ElementHost elementHost1;
        private ButterflyControl butterflyControl1;

    }
}