/*
  TODO

    SPDX-License-Identifier: GPL-2.0-or-later
*/

"use strict";

// TODO: fix style, move initializatio to separate functions

const desktopToScreenState = new Map();
workspace.desktops.forEach((desk, i) => {
    // TODO: for some reason `readConfig` below always returns `undefined`. So
    // screen indices are hardcoded for now.

    // const configKey = `ScreenForDesktop${i + 1}`;
    // let displayIndex = readConfig(configKey); // TBM
    // desktopToScreenState.set(desk, [displayIndex, desk]);
    desktopToScreenState.set(desk, [1,4,6,8].includes(i)? 1 : 0);
});

// TODO: unhardcode these once config debacle is sorted out. Should be the
// current focused desktop/screen + any desktop for other indices. And similar
// for desktopToScreenState.
const activeScreenDesktops = [workspace.desktops[0], workspace.desktops[1]]

const initialCurrent = workspace.currentDesktop;
const initialScreen = desktopToScreenState.get(initialCurrent);
activeScreenDesktops[initialScreen] = initialCurrent;

const isSpecialWindow = win => win.onAllDesktops === true && win.caption === ""

const windowToOrigDesktop = new Map();
workspace.windowList().forEach(win => {
    if (! isSpecialWindow(win) && win.desktops.length == 1) // TODO: ignoring windows on multiple desktops
        windowToOrigDesktop.set(win, win.desktops[0]);
});

function applyVisibility() {
    workspace.windowList().forEach(win => {
        if (isSpecialWindow(win))
            return;

        const origDesktop = windowToOrigDesktop.get(win);
        console.assert(origDesktop != null);
        if (activeScreenDesktops.indexOf(origDesktop) === -1) {
            print(`Returning to ${origDesktop} window "${win.caption}"`)
            win.desktops = [origDesktop];
        } else {
            print(`Making "all desktops" window "${win.caption}"`)
            win.onAllDesktops = true;
        }
    });
}

function onCurrentDesktopChanged(_) {
    const currDesktop = workspace.currentDesktop;
    const iScreen = desktopToScreenState.get(currDesktop);
    activeScreenDesktops[iScreen] = currDesktop;
    applyVisibility();
    // TODO: should focus the screen — but missing API?
}

applyVisibility();
workspace.currentDesktopChanged.connect(onCurrentDesktopChanged);
workspace.windowAdded.connect((win) => {
    if (! isSpecialWindow(win)) {
        windowToOrigDesktop.set(win, win.desktops[0]);
    }
    applyVisibility();
});
workspace.windowRemoved.connect((win) => {
    windowToOrigDesktop.delete(win);
});

// TODO: `onDisable` API to put windows back to original desktops?
