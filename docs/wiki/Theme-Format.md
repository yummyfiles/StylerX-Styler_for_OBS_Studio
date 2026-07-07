# Theme Format

Themes are stored as JSON files. Each file represents one complete theme.

## File location

```
%APPDATA%/StylerX/themes/
```

On most systems that's `C:\Users\YourName\AppData\Roaming\StylerX\themes\`.

## Schema

```json
{
  "format_version": 1,
  "id": "550e8400-e29b-41d4-a716-446655440000",
  "name": "My Custom Theme",
  "author": "your name",
  "is_read_only": false,
  "is_favorite": true,
  "created": "2026-07-06T17:00:00Z",
  "modified": "2026-07-06T18:30:00Z",
  "colors": {
    "background": "#1e1e1e",
    "panels": "#252526",
    "dock_background": "#2d2d30",
    "buttons": "#3c3c3c",
    "button_hover": "#505050",
    "button_pressed": "#2d2d2d",
    "borders": "#3c3c3c",
    "accent": "#0078d4",
    "selected": "#264f78",
    "text": "#cccccc",
    "disabled_text": "#6c6c6c",
    "menu_background": "#2d2d30",
    "menu_text": "#cccccc",
    "tooltips": "#3c3c3c",
    "scrollbars": "#424242",
    "sliders": "#424242",
    "checkboxes": "#3c3c3c",
    "input_fields": "#3c3c3c",
    "tabs": "#2d2d30",
    "status_bar": "#007acc"
  }
}
```

### Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `format_version` | integer | yes | Schema version. Currently `1`. |
| `id` | string | yes | UUID v4 that uniquely identifies the theme. |
| `name` | string | yes | Display name shown in the theme list. |
| `author` | string | no | Who created the theme. |
| `is_read_only` | boolean | no | If true, the theme can't be edited/deleted. Default `false`. |
| `is_favorite` | boolean | no | If true, shows a heart icon in the list. Default `false`. |
| `created` | string (ISO 8601) | no | When the theme was first created. |
| `modified` | string (ISO 8601) | no | When the theme was last changed. |
| `colors` | object | yes | Map of color keys to hex strings. |

### Color format

All colors use hex notation:
- `#RRGGBB` — opaque color
- `#AARRGGBB` — color with alpha channel

Only the 20 keys listed above are recognized. Unknown keys are ignored on import.

## Versioning

`format_version` is currently `1`. If the schema changes in the future, the plugin checks this field on import and will reject files with an unsupported version. This ensures backward compatibility — old themes always work.
