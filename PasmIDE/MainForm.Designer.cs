namespace PasmIDE
{
    partial class MainForm
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.UpperBorder = new System.Windows.Forms.Panel();
            this.closeButton = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.codeBox = new System.Windows.Forms.RichTextBox();
            this.messageList = new System.Windows.Forms.ListBox();
            this.codeOpenDialog = new System.Windows.Forms.OpenFileDialog();
            this.saveSetting = new System.Windows.Forms.Button();
            this.compileAndRun = new System.Windows.Forms.Button();
            this.executeButton = new System.Windows.Forms.Button();
            this.compileButton = new System.Windows.Forms.Button();
            this.vmPath = new System.Windows.Forms.Button();
            this.compilerPath = new System.Windows.Forms.Button();
            this.saveFile = new System.Windows.Forms.Button();
            this.openDocument = new System.Windows.Forms.Button();
            this.exeFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.UpperBorder.SuspendLayout();
            this.SuspendLayout();
            // 
            // UpperBorder
            // 
            this.UpperBorder.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(43)))), ((int)(((byte)(43)))), ((int)(((byte)(43)))));
            this.UpperBorder.Controls.Add(this.closeButton);
            this.UpperBorder.Controls.Add(this.label3);
            this.UpperBorder.Dock = System.Windows.Forms.DockStyle.Top;
            this.UpperBorder.Location = new System.Drawing.Point(0, 0);
            this.UpperBorder.Name = "UpperBorder";
            this.UpperBorder.Size = new System.Drawing.Size(684, 34);
            this.UpperBorder.TabIndex = 8;
            this.UpperBorder.MouseMove += new System.Windows.Forms.MouseEventHandler(this.UpperBorder_MouseMove);
            // 
            // closeButton
            // 
            this.closeButton.Dock = System.Windows.Forms.DockStyle.Right;
            this.closeButton.FlatAppearance.BorderSize = 0;
            this.closeButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.closeButton.Image = ((System.Drawing.Image)(resources.GetObject("closeButton.Image")));
            this.closeButton.Location = new System.Drawing.Point(650, 0);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(34, 34);
            this.closeButton.TabIndex = 1;
            this.closeButton.UseVisualStyleBackColor = true;
            this.closeButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Century Gothic", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.label3.Location = new System.Drawing.Point(7, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 20);
            this.label3.TabIndex = 0;
            this.label3.Text = "PasmIDE";
            // 
            // codeBox
            // 
            this.codeBox.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.codeBox.Location = new System.Drawing.Point(6, 70);
            this.codeBox.Name = "codeBox";
            this.codeBox.Size = new System.Drawing.Size(672, 501);
            this.codeBox.TabIndex = 9;
            this.codeBox.Text = "";
            // 
            // messageList
            // 
            this.messageList.FormattingEnabled = true;
            this.messageList.Location = new System.Drawing.Point(6, 577);
            this.messageList.Name = "messageList";
            this.messageList.Size = new System.Drawing.Size(672, 134);
            this.messageList.TabIndex = 10;
            // 
            // saveSetting
            // 
            this.saveSetting.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_settings_32px;
            this.saveSetting.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.saveSetting.FlatAppearance.BorderSize = 0;
            this.saveSetting.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.saveSetting.Location = new System.Drawing.Point(309, 44);
            this.saveSetting.Name = "saveSetting";
            this.saveSetting.Size = new System.Drawing.Size(20, 20);
            this.saveSetting.TabIndex = 18;
            this.saveSetting.UseVisualStyleBackColor = true;
            // 
            // compileAndRun
            // 
            this.compileAndRun.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_sandbox_32px;
            this.compileAndRun.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.compileAndRun.FlatAppearance.BorderSize = 0;
            this.compileAndRun.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.compileAndRun.Location = new System.Drawing.Point(172, 44);
            this.compileAndRun.Name = "compileAndRun";
            this.compileAndRun.Size = new System.Drawing.Size(20, 20);
            this.compileAndRun.TabIndex = 17;
            this.compileAndRun.UseVisualStyleBackColor = true;
            // 
            // executeButton
            // 
            this.executeButton.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_start_32px;
            this.executeButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.executeButton.FlatAppearance.BorderSize = 0;
            this.executeButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.executeButton.Location = new System.Drawing.Point(146, 44);
            this.executeButton.Name = "executeButton";
            this.executeButton.Size = new System.Drawing.Size(20, 20);
            this.executeButton.TabIndex = 16;
            this.executeButton.UseVisualStyleBackColor = true;
            this.executeButton.Click += new System.EventHandler(this.ExecuteButton_Click);
            // 
            // compileButton
            // 
            this.compileButton.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_hammer_32px;
            this.compileButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.compileButton.FlatAppearance.BorderSize = 0;
            this.compileButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.compileButton.Location = new System.Drawing.Point(120, 44);
            this.compileButton.Name = "compileButton";
            this.compileButton.Size = new System.Drawing.Size(20, 20);
            this.compileButton.TabIndex = 15;
            this.compileButton.UseVisualStyleBackColor = true;
            this.compileButton.Click += new System.EventHandler(this.CompileButton_Click);
            // 
            // vmPath
            // 
            this.vmPath.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_processor_32px;
            this.vmPath.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.vmPath.FlatAppearance.BorderSize = 0;
            this.vmPath.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.vmPath.Location = new System.Drawing.Point(283, 44);
            this.vmPath.Name = "vmPath";
            this.vmPath.Size = new System.Drawing.Size(20, 20);
            this.vmPath.TabIndex = 14;
            this.vmPath.UseVisualStyleBackColor = true;
            this.vmPath.Click += new System.EventHandler(this.VmPath_Click);
            // 
            // compilerPath
            // 
            this.compilerPath.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_maintenance_32px;
            this.compilerPath.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.compilerPath.FlatAppearance.BorderSize = 0;
            this.compilerPath.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.compilerPath.Location = new System.Drawing.Point(257, 44);
            this.compilerPath.Name = "compilerPath";
            this.compilerPath.Size = new System.Drawing.Size(20, 20);
            this.compilerPath.TabIndex = 13;
            this.compilerPath.UseVisualStyleBackColor = true;
            this.compilerPath.Click += new System.EventHandler(this.CompilerPath_Click);
            // 
            // saveFile
            // 
            this.saveFile.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_save_32px;
            this.saveFile.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.saveFile.FlatAppearance.BorderSize = 0;
            this.saveFile.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.saveFile.Location = new System.Drawing.Point(37, 44);
            this.saveFile.Name = "saveFile";
            this.saveFile.Size = new System.Drawing.Size(20, 20);
            this.saveFile.TabIndex = 12;
            this.saveFile.UseVisualStyleBackColor = true;
            this.saveFile.Click += new System.EventHandler(this.SaveFile_Click);
            // 
            // openDocument
            // 
            this.openDocument.BackgroundImage = global::PasmIDE.Properties.Resources.icons8_filing_32px;
            this.openDocument.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.openDocument.FlatAppearance.BorderSize = 0;
            this.openDocument.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.openDocument.Location = new System.Drawing.Point(11, 44);
            this.openDocument.Name = "openDocument";
            this.openDocument.Size = new System.Drawing.Size(20, 20);
            this.openDocument.TabIndex = 11;
            this.openDocument.UseVisualStyleBackColor = true;
            this.openDocument.Click += new System.EventHandler(this.OpenDocument_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(43)))), ((int)(((byte)(43)))), ((int)(((byte)(43)))));
            this.ClientSize = new System.Drawing.Size(684, 716);
            this.Controls.Add(this.saveSetting);
            this.Controls.Add(this.compileAndRun);
            this.Controls.Add(this.executeButton);
            this.Controls.Add(this.compileButton);
            this.Controls.Add(this.vmPath);
            this.Controls.Add(this.compilerPath);
            this.Controls.Add(this.saveFile);
            this.Controls.Add(this.openDocument);
            this.Controls.Add(this.messageList);
            this.Controls.Add(this.codeBox);
            this.Controls.Add(this.UpperBorder);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "MainForm";
            this.UpperBorder.ResumeLayout(false);
            this.UpperBorder.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel UpperBorder;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.RichTextBox codeBox;
        private System.Windows.Forms.ListBox messageList;
        private System.Windows.Forms.Button openDocument;
        private System.Windows.Forms.Button saveFile;
        private System.Windows.Forms.Button compilerPath;
        private System.Windows.Forms.Button vmPath;
        private System.Windows.Forms.Button compileButton;
        private System.Windows.Forms.Button executeButton;
        private System.Windows.Forms.Button compileAndRun;
        private System.Windows.Forms.Button saveSetting;
        private System.Windows.Forms.OpenFileDialog codeOpenDialog;
        private System.Windows.Forms.OpenFileDialog exeFileDialog;
    }
}

