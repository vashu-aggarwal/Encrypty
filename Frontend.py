import sys
import subprocess
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QPushButton, QFileDialog, QTextEdit, QComboBox, QLineEdit
)
from PyQt5.QtGui import QIcon

class EncryptorApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("File Encryptor/Decryptor")
        self.showMaximized()

        layout = QVBoxLayout()

        # Mode Selector
        mode_layout = QHBoxLayout()
        self.mode_combo = QComboBox()
        self.mode_combo.addItems(["Multi-file", "Single-file"])
        self.mode_combo.currentTextChanged.connect(self.toggle_mode)
        mode_layout.addWidget(QLabel("Mode:"))
        mode_layout.addWidget(self.mode_combo)
        layout.addLayout(mode_layout)

        # Folder input layout
        self.folder_layout = QHBoxLayout()
        self.folder_input = QLineEdit()
        self.folder_input.setPlaceholderText("Select folder containing files")
        folder_browse = QPushButton("Browse")
        folder_browse.clicked.connect(self.browse_folder)
        self.folder_layout.addWidget(QLabel("Folder:"))
        self.folder_layout.addWidget(self.folder_input)
        self.folder_layout.addWidget(folder_browse)
        layout.addLayout(self.folder_layout)

        # Password input layout (for multi-file mode only)
        self.password_layout = QHBoxLayout()
        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.Password)
        self.password_input.setPlaceholderText("Enter password for encryption/decryption")
        self.password_layout.addWidget(QLabel("Password:"))
        self.password_layout.addWidget(self.password_input)
        layout.addLayout(self.password_layout)

        # File input layout
        self.file_layout = QHBoxLayout()
        self.file_input = QLineEdit()
        self.file_input.setPlaceholderText("Select single file")
        file_browse = QPushButton("Browse")
        file_browse.clicked.connect(self.browse_file)
        self.file_layout.addWidget(QLabel("File:"))
        self.file_layout.addWidget(self.file_input)
        self.file_layout.addWidget(file_browse)
        layout.addLayout(self.file_layout)

        # Action Selector
        action_layout = QHBoxLayout()
        self.action_combo = QComboBox()
        self.action_combo.addItems(["encrypt", "decrypt"])
        action_layout.addWidget(QLabel("Action:"))
        action_layout.addWidget(self.action_combo)
        layout.addLayout(action_layout)

        # Run Button
        button_layout = QHBoxLayout()
        run_button = QPushButton("Run")
        run_button.clicked.connect(self.run_process)
        button_layout.addStretch()
        button_layout.addWidget(run_button)
        layout.addLayout(button_layout)

        # Output Log
        output_label = QLabel("Output:")
        output_label.setStyleSheet("margin-top: 10px;")
        layout.addWidget(output_label)

        self.output_box = QTextEdit()
        self.output_box.setReadOnly(True)
        self.output_box.setFixedHeight(250)
        layout.addWidget(self.output_box)

        self.setLayout(layout)
        self.toggle_mode("Multi-file")  # default view

        self.setStyleSheet("""
            QWidget {
                font-family: 'Segoe UI', sans-serif;
                font-size: 16px;
                background-color: #332D56;
                color: #E3EEB2;
            }

            QLabel {
                font-weight: bold;
                font-size: 16px;
                color: #E3EEB2;
            }

            QLineEdit, QTextEdit, QComboBox {
                background-color: #4E6688;
                border: 1px solid #71C0BB;
                border-radius: 6px;
                padding: 10px;
                font-size: 16px;
                color: #E3EEB2;
            }

            QPushButton {
                background-color: #71C0BB;
                border: none;
                padding: 10px 20px;
                font-size: 16px;
                border-radius: 8px;
                color: #332D56;
                font-weight: bold;
            }

            QPushButton:hover {
                background-color: #E3EEB2;
                color: #332D56;
            }

            QPushButton:pressed {
                background-color: #4E6688;
                color: #E3EEB2;
            }

            QTextEdit {
                background-color: #4E6688;
                border: 1px solid #71C0BB;
                border-radius: 6px;
                padding: 10px;
                font-size: 15px;
                color: #E3EEB2;
            }
        """)

    def toggle_mode(self, mode):
        is_multi = (mode == "Multi-file")
        self.folder_input.setVisible(is_multi)
        self.folder_layout.itemAt(0).widget().setVisible(is_multi)
        self.folder_layout.itemAt(2).widget().setVisible(is_multi)

        self.password_input.setVisible(is_multi)
        self.password_layout.itemAt(0).widget().setVisible(is_multi)

        self.file_input.setVisible(not is_multi)
        self.file_layout.itemAt(0).widget().setVisible(not is_multi)
        self.file_layout.itemAt(2).widget().setVisible(not is_multi)

    def browse_folder(self):
        folder = QFileDialog.getExistingDirectory(self, "Select Folder")
        if folder:
            self.folder_input.setText(folder)

    def browse_file(self):
        file, _ = QFileDialog.getOpenFileName(self, "Select File")
        if file:
            self.file_input.setText(file)

    def run_process(self):
        mode = self.mode_combo.currentText()
        action = self.action_combo.currentText().strip()

        try:
            if mode == "Multi-file":
                folder = self.folder_input.text().strip()
                password = self.password_input.text().strip()

                if not folder:
                    self.output_box.append("❌ Please select a folder first.")
                    return
                if not password:
                    self.output_box.append("❌ Please enter a password.")
                    return

                process = subprocess.Popen(
                    ['./encrypt_decrypt'],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True
                )
                stdout, stderr = process.communicate(input=f"{folder}\n{action}\n{password}\n")

            else:  # Single-file
                file = self.file_input.text().strip()
                if not file:
                    self.output_box.append("❌ Please select a file first.")
                    return

                process = subprocess.Popen(
                    ['./mmap', action, file],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True
                )
                stdout, stderr = process.communicate()

            if stdout:
                self.output_box.append("✅ Output:\n" + stdout)
            if stderr:
                self.output_box.append("⚠️ Errors:\n" + stderr)

        except FileNotFoundError:
            binary = "encrypt_decrypt" if mode == "Multi-file" else "mmap"
            self.output_box.append(f"❌ Could not find the {binary} binary.")
        except Exception as e:
            self.output_box.append(f"❌ An error occurred: {str(e)}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = EncryptorApp()
    window.show()
    sys.exit(app.exec_())
