# Usage

## Opening StylerX

1. Launch OBS Studio
2. Go to **Tools → StylerX Studio**
3. A dock panel appears with three tabs: Colors, QSS, and Inspector

## The Colors tab

Adjust 20+ UI elements. Each one has:

- **Hex input** — type a hex color like `#ff6600` or `#aabbccdd` (with alpha)
- **RGB sliders** — Red, Green, Blue from 0–255
- **HSV sliders** — Hue (0–360), Saturation (0–100), Value (0–100)
- **Alpha slider** — transparency from 0 (invisible) to 255 (solid)
- **Color picker button** — opens the system color dialog

All controls stay in sync — change one and the others update.

### Available color slots

| Key | What it affects |
|-----|----------------|
| `background` | Main dark background |
| `panels` | Side panels and containers |
| `dock_background` | Inside of dock widgets |
| `buttons` | Push buttons and tool buttons |
| `button_hover` | Buttons on mouse hover |
| `button_pressed` | Buttons while clicked |
| `borders` | Separator lines and outlines |
| `accent` | Primary highlight color |
| `selected` | Selected list/tree items |
| `text` | Main text color |
| `disabled_text` | Text on disabled controls |
| `menu_background` | Menu bar and dropdown menus |
| `menu_text` | Text inside menus |
| `tooltips` | Tooltip popup backgrounds |
| `scrollbars` | Scrollbar handles |
| `sliders` | Slider controls |
| `checkboxes` | Checkbox and radio button backgrounds |
| `input_fields` | Text inputs, spinboxes, comboboxes |
| `tabs` | Tab bar backgrounds |
| `status_bar` | OBS status bar |

## The QSS tab

A full code editor for the raw stylesheet. Features:

- Syntax highlighting (selectors, properties, values, comments, hex colors)
- Line numbers in the gutter
- Search (<kbd>Ctrl+F</kbd>) and replace (<kbd>Ctrl+H</kbd>)
- Replace All button
- <kbd>Ctrl+S</kbd> to apply the stylesheet to OBS immediately
- Cursor position display (line and column)

Use this for things the color picker can't do: border radii, gradients, padding, font sizes, targeting specific widget classes.

## The Inspector tab

1. Check **Enable Inspector Mode**
2. Hover over any widget in OBS — it gets highlighted with an orange border
3. Click on it — you'll see:
   - Class name (e.g. `QPushButton`, `QListWidget`)
   - Object name (if it has one)
   - Current stylesheet rules
   - All hex and RGB colors found in the stylesheet
4. Click **Edit These Colors** to jump to the Colors tab

## Managing themes

### Theme list sidebar

On the left side of the dock is your theme list. Use the search bar at the top to filter.

### Buttons

| Button | Action |
|--------|--------|
| `+` | Create a new blank theme |
| `✓` | Save current theme to disk |
| `D` | Duplicate the selected theme |
| `X` | Delete the selected theme |
| **Export** | Save theme as a `.json` file |
| **Import** | Load a `.json` theme file |
| **Reset** | Clear active theme back to OBS defaults |

### Right-click context menu

Right-click any theme for: Rename, Favorite/Unfavorite, Duplicate, Export, Delete (editable themes only).

### Read-only themes

Default/built-in themes have a lock icon. You can't delete or overwrite them — duplicate first, then edit the copy.
