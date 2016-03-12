.pragma library

function fromText(str){
    return icons[str] || ""
}

var icons = {
    'search':       "\uf002",
    'fav':          "\uf004",
    'game':         "\uf11b",
    'chevron_r':    "\uf054",
    'chevron_l':    "\uf053",
    'spinner':      "\uf1ce",
    'play':         "\uf04b",
    'pause':        "\uf04c",
    'remove':       "\uf00d",
    'info':         "\uf129",
    'featured':     "\uf0a1",
    'settings':     "\uf013",
    'check':        "\uf00c",
    'volume':       "\uf028",
    'volume-off':   "\uf026",
    'volume-med':   "\uf027",
    'video':        "\uf03d"
}
