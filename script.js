// Lire et décoder les cookies
document.addEventListener("DOMContentLoaded", function () {
    const cookies = document.cookie.split("; ").reduce((acc, cookie) => {
        const [key, value] = cookie.split("=");
        acc[key] = decodeURIComponent(value);
        return acc;
    }, {});

    // Extraire le cookie 'username'
    const username = cookies["username"];

    if (username) {
        // Afficher le username s'il existe
        const welcomeUsername = document.getElementById("welcome-username");
        welcomeUsername.textContent = `👤 ${username}`;
        welcomeUsername.classList.remove("hidden");
    }
});
