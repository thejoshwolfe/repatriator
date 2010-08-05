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
            this.chatInputText = new System.Windows.Forms.TextBox();
            this.hostPortText = new System.Windows.Forms.TextBox();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(205, 12);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(75, 23);
            this.connectButton.TabIndex = 2;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // hostNameText
            // 
            this.hostNameText.Location = new System.Drawing.Point(12, 12);
            this.hostNameText.Name = "hostNameText";
            this.hostNameText.Size = new System.Drawing.Size(141, 20);
            this.hostNameText.TabIndex = 3;
            this.hostNameText.Text = "localhost";
            // 
            // chatInputText
            // 
            this.chatInputText.Enabled = false;
            this.chatInputText.Location = new System.Drawing.Point(12, 41);
            this.chatInputText.Name = "chatInputText";
            this.chatInputText.Size = new System.Drawing.Size(268, 20);
            this.chatInputText.TabIndex = 5;
            this.chatInputText.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.chatInputText_KeyPress);
            // 
            // hostPortText
            // 
            this.hostPortText.Location = new System.Drawing.Point(159, 12);
            this.hostPortText.Name = "hostPortText";
            this.hostPortText.Size = new System.Drawing.Size(40, 20);
            this.hostPortText.TabIndex = 7;
            this.hostPortText.Text = "9999";
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(12, 67);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(268, 195);
            this.pictureBox.TabIndex = 8;
            this.pictureBox.TabStop = false;
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 274);
            this.Controls.Add(this.pictureBox);
            this.Controls.Add(this.hostPortText);
            this.Controls.Add(this.chatInputText);
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
        private System.Windows.Forms.TextBox chatInputText;
        private System.Windows.Forms.TextBox hostPortText;
        private System.Windows.Forms.PictureBox pictureBox;

    }
}