# TeleRynda

![TeleRynda Logo](.github/AyuGram.png) ![AyuChan](.github/AyuChan.png)

[ English  |   [Русский](README-RU.md) ]

## Features

- Full ghost mode (flexible)
- Messages history
- Anti-recall
- Font customization
- Streamer mode
- Local Telegram Premium
- Translator
- Media preview & quick reaction on force click (macOS)
- Enhanced appearance

And many more. Check out our [Documentation](https://docs.ayugram.one/desktop/).

<h3>
  <details>
    <summary>Preview</summary>
    <table>
      <tr>
        <td><img src='.github/demos/demo1.png' width='268' alt='Preferences'></td>
        <td><img src='.github/demos/demo2.png' width='268' alt='TeleRynda Options'></td>
        <td><img src='.github/demos/demo3.png' width='268' alt='Message Filters'></td>
      </tr>
      <tr>
        <td><img src='.github/demos/demo4.png' width='268' alt='Appearance'></td>
        <td><img src='.github/demos/demo5.png' width='268' alt='Chats'></td>
      </tr>
    </table>
  </details>
</h3>

## Downloads

### Windows

#### Official

You can download prebuilt Windows binary from [Releases tab](https://github.com/TeleRynda/TeleRyndaDesktop/releases) or from
the [Telegram channel](https://t.me/TeleRyndaReleases).

#### Winget

```bash
winget install RadolynLabs.TeleRyndaDesktop
```

#### Scoop

```bash
scoop bucket add extras
scoop install telerynda
```

#### Self-built

Follow [official guide](https://github.com/TeleRynda/TeleRyndaDesktop/blob/dev/docs/building-win-x64.md) if you want to
build by yourself.

### macOS

#### Official

You can download prebuilt macOS package from [Releases tab](https://github.com/TeleRynda/TeleRyndaDesktop/releases).

#### Homebrew

```bash
brew install --cask telerynda
```

### Arch Linux

#### From source (recommended)

Install `telerynda-desktop` from [AUR](https://aur.archlinux.org/packages/telerynda-desktop).

#### Prebuilt binaries

Install `telerynda-desktop-bin` from [AUR](https://aur.archlinux.org/packages/telerynda-desktop-bin).

Note: these binaries aren't officially maintained by us.

### NixOS

See [this repository](https://github.com/telerynda-port/telerynda-desktop) for installation manual.

### ALT Linux

[Sisyphus](https://packages.altlinux.org/en/sisyphus/srpms/telerynda-desktop/)

### EPM

`epm play telerynda`

### Any other Linux distro

Flatpak: https://github.com/0FL01/TeleRyndaDesktop-flatpak

Or follow the [official guide](https://github.com/TeleRynda/TeleRyndaDesktop/blob/dev/docs/building-linux.md).

### Remarks for Windows

Make sure you have these components installed with VS Build Tools:

- C++ MFC latest (x86 & x64)
- C++ ATL latest (x86 & x64)
- latest Windows 11 SDK

## Donation

Enjoy using **TeleRynda**? Consider sending us a tip!

[Here's available methods.](https://docs.ayugram.one/donate/)

## Credits

### Telegram clients

- [Telegram Desktop](https://github.com/telegramdesktop/tdesktop)
- [Kotatogram](https://github.com/kotatogram/kotatogram-desktop)
- [64Gram](https://github.com/TDesktop-x64/tdesktop)
- [Forkgram](https://github.com/forkgram/tdesktop)

### Libraries used

- [JSON for Modern C++](https://github.com/nlohmann/json)
- [SQLite](https://github.com/sqlite/sqlite)
- [sqlite_orm](https://github.com/fnc12/sqlite_orm)
- [androidx sources](https://github.com/androidx/androidx)

### Icons

- [Solar Icon Set](https://www.figma.com/community/file/1166831539721848736)

### Bots

- [TelegramDB](https://t.me/tgdatabase) for username lookup by ID
