function isDocked()
{
    return content.state === "docked"
}

function setDocked(docked)
{
    content.state = docked ? "docked" : "undocked"
}