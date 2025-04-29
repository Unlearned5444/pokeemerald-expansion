#!/usr/bin/env python3
import wx
import wx.grid
import os
import re

class PachisiLevelMakerFrame(wx.Frame):
    def __init__(self, parent, title):
        super(PachisiLevelMakerFrame, self).__init__(parent, title=title, size=(900, 600))
        self.level_file = None  # stores the current level file path if any
        self.square_types = []
        self.load_defines()
        
        # Create an internal board model with 50 rows (each as a dict)
        self.board_data = [
            {
                'index': i,
                'square': "SQUARE_NONE",
                'data0': "0",
                'data1': "0",
                'data2': "0"
            }
            for i in range(50)
        ]
        
        self.current_mode = "Grid View"  # default mode (align with choice options)
        self.init_ui()
        self.create_menu()
        self.Centre()

    def load_defines(self):
        """
        Read the 'pachisi_defines.h' file and grab all SQUARE_ definitions
        (except NUM_NORMAL_SQUARES) to use for the drop‐down choice.
        """
        defines_file = "pachisi_defines.h"
        try:
            with open(defines_file, "r") as f:
                content = f.read()
        except Exception as e:
            wx.MessageBox(f"Error reading {defines_file}: {e}", "Error", wx.ICON_ERROR)
            content = ""
        self.square_types = []
        for line in content.splitlines():
            line = line.strip()
            if line.startswith("#define"):
                parts = line.split()
                if len(parts) < 3:
                    continue
                name = parts[1]
                # Only include macros that start with SQUARE_ and aren’t the count macro.
                if name.startswith("SQUARE_") and name != "NUM_NORMAL_SQUARES":
                    self.square_types.append(name)
        # Remove duplicates while preserving order
        seen = set()
        self.square_types = [x for x in self.square_types if not (x in seen or seen.add(x))]

    def init_ui(self):
        """
        Build the UI:
         - A control panel with square type selection, three data entry fields,
           an Apply button, an interface mode dropdown, and a new Resize Board button.
         - A board panel where either a grid (standard mode) or list (accessible mode)
           is displayed.
         - All controls are added in order so that tab navigation flows correctly.
        """
        panel = wx.Panel(self)
        mainSizer = wx.BoxSizer(wx.VERTICAL)

        # ---- Control Panel ----
        ctrlSizer = wx.BoxSizer(wx.HORIZONTAL)
        
        # Square type selection
        ctrlSizer.Add(wx.StaticText(panel, label="Square Type:"), 0, wx.ALL | wx.CENTER, 5)
        self.squareTypeChoice = wx.Choice(panel, choices=self.square_types)
        ctrlSizer.Add(self.squareTypeChoice, 0, wx.ALL | wx.CENTER, 5)
        
        # Data text controls
        self.dataCtrls = []
        for i in range(3):
            ctrlSizer.Add(wx.StaticText(panel, label=f"Data{i}:"), 0, wx.ALL | wx.CENTER, 5)
            tc = wx.TextCtrl(panel)
            ctrlSizer.Add(tc, 0, wx.ALL | wx.CENTER, 5)
            self.dataCtrls.append(tc)
        
        # Apply changes button
        applyBtn = wx.Button(panel, label="Apply to Selected")
        applyBtn.Bind(wx.EVT_BUTTON, self.on_apply)
        ctrlSizer.Add(applyBtn, 0, wx.ALL | wx.CENTER, 5)

        # Interface mode dropdown for switching between Grid View and List View
        ctrlSizer.Add(wx.StaticText(panel, label="Interface Mode:"), 0, wx.ALL | wx.CENTER, 5)
        self.accessibilityModeChoice = wx.Choice(panel, choices=["Grid View", "List View"])
        # Set the default selection to Grid View
        self.accessibilityModeChoice.SetSelection(0)
        self.accessibilityModeChoice.Bind(wx.EVT_CHOICE, self.on_change_interface_mode)
        ctrlSizer.Add(self.accessibilityModeChoice, 0, wx.ALL | wx.CENTER, 5)
        
        # ---- New: Resize Board Button ----
        resizeBtn = wx.Button(panel, label="Resize Board")
        resizeBtn.Bind(wx.EVT_BUTTON, self.on_resize)
        ctrlSizer.Add(resizeBtn, 0, wx.ALL | wx.CENTER, 5)

        mainSizer.Add(ctrlSizer, 0, wx.EXPAND)

        # ---- Board Panel (holds either grid or list) ----
        self.board_panel = wx.Panel(panel, style=wx.TAB_TRAVERSAL)
        boardSizer = wx.BoxSizer(wx.VERTICAL)
        self.board_panel.SetSizer(boardSizer)
        
        # Create the initial board view based on current interface mode.
        self.create_board_control(self.current_mode)
        mainSizer.Add(self.board_panel, 1, wx.EXPAND | wx.ALL, 5)
        
        panel.SetSizer(mainSizer)
        self.CreateStatusBar()

    def create_board_control(self, mode):
        """
        Create the board view control based on the chosen mode.
         - "Grid View" uses a wx.Grid.
         - "List View" uses a wx.ListCtrl in report mode.
        """
        # Clear any existing children in board_panel.
        for child in self.board_panel.GetChildren():
            child.Destroy()
        
        if mode == "Grid View":
            self.board_control = wx.grid.Grid(self.board_panel)
            # Create grid with rows = number of board_data items, 5 columns.
            self.board_control.CreateGrid(len(self.board_data), 5)
            # Set column labels.
            self.board_control.SetColLabelValue(0, "Index")
            self.board_control.SetColLabelValue(1, "Square")
            self.board_control.SetColLabelValue(2, "Data0")
            self.board_control.SetColLabelValue(3, "Data1")
            self.board_control.SetColLabelValue(4, "Data2")
            self.board_control.SetColSize(0, 50)
            # Mark the Index column read-only.
            for row in range(self.board_control.GetNumberRows()):
                self.board_control.SetReadOnly(row, 0, True)
            # Set cell editor for Square column.
            attr = wx.grid.GridCellAttr()
            editor = wx.grid.GridCellChoiceEditor(self.square_types, allowOthers=True)
            attr.SetEditor(editor)
            self.board_control.SetColAttr(1, attr)
        else:
            # Accessible mode: use wx.ListCtrl in report mode.
            self.board_control = wx.ListCtrl(self.board_panel, style=wx.LC_REPORT )
            self.board_control.InsertColumn(0, "Index", width=50)
            self.board_control.InsertColumn(1, "Square", width=150)
            self.board_control.InsertColumn(2, "Data0", width=100)
            self.board_control.InsertColumn(3, "Data1", width=100)
            self.board_control.InsertColumn(4, "Data2", width=100)
            # Make sure the list is focusable
            self.board_control.SetFocus()
        
        self.refresh_board_view()
        # Add the control to the board panel's sizer.
        self.board_panel.GetSizer().Add(self.board_control, 1, wx.EXPAND)
        self.board_panel.Layout()

    def refresh_board_view(self):
        """
        Update the current board view control with the data from self.board_data.
        """
        if isinstance(self.board_control, wx.grid.Grid):
            # Adjust grid rows if needed.
            current_rows = self.board_control.GetNumberRows()
            needed = len(self.board_data)
            if needed < current_rows:
                self.board_control.DeleteRows(needed, current_rows - needed)
            elif needed > current_rows:
                self.board_control.AppendRows(needed - current_rows)
            # Populate grid cells.
            for row, item in enumerate(self.board_data):
                self.board_control.SetCellValue(row, 0, str(item['index']))
                self.board_control.SetCellValue(row, 1, item['square'])
                self.board_control.SetCellValue(row, 2, item['data0'])
                self.board_control.SetCellValue(row, 3, item['data1'])
                self.board_control.SetCellValue(row, 4, item['data2'])
                # Make index column read-only.
                self.board_control.SetReadOnly(row, 0, True)
        elif isinstance(self.board_control, wx.ListCtrl):
            self.board_control.DeleteAllItems()
            for item in self.board_data:
                idx = self.board_control.InsertItem(self.board_control.GetItemCount(), str(item['index']))
                self.board_control.SetItem(idx, 1, item['square'])
                self.board_control.SetItem(idx, 2, item['data0'])
                self.board_control.SetItem(idx, 3, item['data1'])
                self.board_control.SetItem(idx, 4, item['data2'])

    def save_current_board_state(self):
        """
        Update the internal board_data by reading the current board control.
        """
        if isinstance(self.board_control, wx.grid.Grid):
            rows = self.board_control.GetNumberRows()
            for row in range(rows):
                self.board_data[row]['square'] = self.board_control.GetCellValue(row, 1).strip()
                self.board_data[row]['data0'] = self.board_control.GetCellValue(row, 2).strip()
                self.board_data[row]['data1'] = self.board_control.GetCellValue(row, 3).strip()
                self.board_data[row]['data2'] = self.board_control.GetCellValue(row, 4).strip()
        elif isinstance(self.board_control, wx.ListCtrl):
            count = self.board_control.GetItemCount()
            for row in range(count):
                self.board_data[row]['square'] = self.board_control.GetItemText(row, 1).strip()
                # ListCtrl does not have a direct method to get subitems so we use GetItem which returns a wx.ListItem.
                item = wx.ListItem()
                item.SetId(row)
                item.SetColumn(2)
                self.board_data[row]['data0'] = self.board_control.GetItem(row, 2)
                self.board_data[row]['data1'] = self.board_control.GetItem(row, 3)
                self.board_data[row]['data2'] = self.board_control.GetItem(row, 4)

    def on_change_interface_mode(self, event):
        """
        Handle the event when the user changes the interface mode.
        Save current board state, then recreate the board control accordingly.
        """
        new_mode = self.accessibilityModeChoice.GetStringSelection()
        if new_mode not in ["Grid View", "List View"]:
            new_mode = "Grid View"
        # Save current state
        self.save_current_board_state()
        self.current_mode = new_mode
        # Recreate the board control in the chosen mode.
        self.create_board_control(self.current_mode)
        self.SetStatusText(f"Switched to {self.current_mode} mode.")

    def on_apply(self, event):
        """
        Update selected board squares based on controls.
        This method checks if the current board control is grid or list.
        """
        square_choice = self.squareTypeChoice.GetStringSelection()
        if not square_choice:
            wx.MessageBox("Please select a square type.", "Info", wx.ICON_INFORMATION)
            return

        data_values = [ctrl.GetValue().strip() for ctrl in self.dataCtrls]

        selected_indices = []
        if isinstance(self.board_control, wx.grid.Grid):
            selected_indices = self.board_control.GetSelectedRows()
        elif isinstance(self.board_control, wx.ListCtrl):
            index = self.board_control.GetFirstSelected()
            while index != -1:
                selected_indices.append(index)
                index = self.board_control.GetNextSelected(index)
        
        if not selected_indices:
            wx.MessageBox("No rows selected.", "Info", wx.ICON_INFORMATION)
            return
        
        for idx in selected_indices:
            self.board_data[idx]['square'] = square_choice
            for i in range(3):
                # Use entered value or default to "0"
                self.board_data[idx][f'data{i}'] = data_values[i] if data_values[i] != "" else "0"
        self.refresh_board_view()
        self.SetStatusText("Applied changes to selected rows.")

    def on_resize(self, event):
        """
        Prompt the user for a new board size and update the board_data accordingly.
        Warns if reducing size will result in data loss.
        """
        current_size = len(self.board_data)
        dlg = wx.TextEntryDialog(self, "Enter new board size:", "Resize Board", str(current_size))
        if dlg.ShowModal() == wx.ID_OK:
            try:
                new_size = int(dlg.GetValue())
                if new_size <= 0:
                    wx.MessageBox("Please enter a positive integer.", "Error", wx.ICON_ERROR)
                    dlg.Destroy()
                    return
            except ValueError:
                wx.MessageBox("Invalid input. Please enter a valid integer.", "Error", wx.ICON_ERROR)
                dlg.Destroy()
                return
            
            if new_size < current_size:
                confirm = wx.MessageBox("Reducing board size will result in loss of data for rows beyond the new size. Continue?",
                                          "Warning", wx.YES_NO | wx.ICON_WARNING)
                if confirm != wx.YES:
                    dlg.Destroy()
                    return
            
            # Extend or truncate the board_data accordingly.
            if new_size > current_size:
                for i in range(current_size, new_size):
                    self.board_data.append({
                        'index': i,
                        'square': "SQUARE_NONE",
                        'data0': "0",
                        'data1': "0",
                        'data2': "0"
                    })
            elif new_size < current_size:
                self.board_data = self.board_data[:new_size]
            
            # Update indices
            for i, item in enumerate(self.board_data):
                item['index'] = i
            
            self.refresh_board_view()
            self.SetStatusText(f"Board resized to {new_size} rows.")
        dlg.Destroy()

    def create_menu(self):
        """
        Create a File menu with New, Open, Save, Save As, and Exit.
        """
        menuBar = wx.MenuBar()
        fileMenu = wx.Menu()

        newItem = fileMenu.Append(wx.ID_NEW, "&New...\tCtrl+N", "New level")
        openItem = fileMenu.Append(wx.ID_OPEN, "&Open...\tCtrl+O", "Open level file")
        saveItem = fileMenu.Append(wx.ID_SAVE, "&Save\tCtrl+S", "Save level file")
        saveAsItem = fileMenu.Append(wx.ID_SAVEAS, "Save &As...\tCtrl+Shift+S", "Save level file as")
        fileMenu.AppendSeparator()
        exitItem = fileMenu.Append(wx.ID_EXIT, "E&xit", "Exit application")

        menuBar.Append(fileMenu, "&File")
        self.SetMenuBar(menuBar)

        self.Bind(wx.EVT_MENU, self.on_new, newItem)
        self.Bind(wx.EVT_MENU, self.on_open, openItem)
        self.Bind(wx.EVT_MENU, self.on_save, saveItem)
        self.Bind(wx.EVT_MENU, self.on_save_as, saveAsItem)
        self.Bind(wx.EVT_MENU, self.on_exit, exitItem)
    def on_new(self, event):
        """
        Create a new level: reset all squares to SQUARE_NONE and data fields to 0.
        """
        for item in self.board_data:
            item['square'] = "SQUARE_NONE"
            item['data0'] = "0"
            item['data1'] = "0"
            item['data2'] = "0"
        self.refresh_board_view()
        self.level_file = None
        self.SetStatusText("New level created.")

    def on_open(self, event):
        """
        Open an existing level file (*.h) and update the internal board data.
        """
        wildcard = "Header files (*.h)|*.h|All files (*.*)|*.*"
        dlg = wx.FileDialog(self, "Open level file", wildcard=wildcard,
                            style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST)
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        pathname = dlg.GetPath()
        self.load_level_file(pathname)
        dlg.Destroy()

    def load_level_file(self, pathname):
        """
        Parse the level file and update self.board_data.
        Expects the file to contain:
        
          static const PachisiSquare PachisiLevel_name[] =
          {
              { .square = SQUARE_TYPE, .data = { data0, data1, data2 } },
              ...
          };
        """
        try:
            with open(pathname, "r") as f:
                content = f.read()
        except Exception as e:
            wx.MessageBox(f"Failed to open file: {e}", "Error", wx.ICON_ERROR)
            return

        initializer_match = re.search(r'\{(.*)\};', content, re.DOTALL)
        if not initializer_match:
            wx.MessageBox("Invalid level file format.", "Error", wx.ICON_ERROR)
            return
        init_block = initializer_match.group(1)

        entries = re.findall(r'\{([^}]+)\}', init_block)
        if not entries:
            wx.MessageBox("No entries found in level file.", "Error", wx.ICON_ERROR)
            return

        self.board_data = []
        for idx, entry in enumerate(entries):
            entry = entry.strip()
            square = None
            data = ["0", "0", "0"]
            m = re.search(r'\.square\s*=\s*(\w+)', entry)
            if m:
                square = m.group(1)
            else:
                tokens = entry.split(',')
                if tokens:
                    token = tokens[0].strip()
                    m2 = re.search(r'(SQUARE_\w+)', token)
                    if m2:
                        square = m2.group(1)
            for i in range(3):
                m_data = re.search(r'\.data\[' + str(i) + r'\]\s*=\s*([^,\}]+)', entry)
                if m_data:
                    data[i] = m_data.group(1).strip()
            if square is None:
                square = "SQUARE_NONE"
            self.board_data.append({
                'index': idx,
                'square': square,
                'data0': data[0],
                'data1': data[1],
                'data2': data[2]
            })

        # Update the board view according to the current mode.
        self.refresh_board_view()
        self.level_file = pathname
        self.SetStatusText(f"Loaded level from {pathname}")

    def on_save(self, event):
        """
        Save the level to the current file (or ask for a file name if new).
        """
        if self.level_file:
            self.save_level_file(self.level_file)
        else:
            self.on_save_as(event)

    def on_save_as(self, event):
        """
        Ask the user for a file name to save the level.
        """
        wildcard = "Header files (*.h)|*.h|All files (*.*)|*.*"
        dlg = wx.FileDialog(self, "Save level file as...", wildcard=wildcard,
                            style=wx.FD_SAVE | wx.FD_OVERWRITE_PROMPT)
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        pathname = dlg.GetPath()
        self.save_level_file(pathname)
        dlg.Destroy()

    def save_level_file(self, pathname):
        """
        Write self.board_data into a header file in the desired C initializer format.
        """
        base = os.path.basename(pathname)
        name, _ = os.path.splitext(base)
        struct_name = f"PachisiLevel_{name.upper()}"
        output = f"/* Generated by pachisi_map_builder.py */\nstatic const PachisiSquare {struct_name}[] =\n{{\n"
        for item in self.board_data:
            output += f"    {{ .square = {item['square']}, .data = {{ {item['data0']}, {item['data1']}, {item['data2']} }} }},\n"
        output += "};\n"
        try:
            with open(pathname, "w") as f:
                f.write(output)
        except Exception as e:
            wx.MessageBox(f"Error saving file: {e}", "Error", wx.ICON_ERROR)
            return
        self.level_file = pathname
        self.SetStatusText(f"Saved level to {pathname}")

    def on_exit(self, event):
        self.Close(True)

def main():
    app = wx.App(False)
    frame = PachisiLevelMakerFrame(None, "Pachisi Level Maker")
    frame.Show()
    app.MainLoop()

if __name__ == "__main__":
    main()
