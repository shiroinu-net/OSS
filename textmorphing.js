// textmorphing.js

document.addEventListener('DOMContentLoaded', () => {
    console.log("DOMContentLoaded: Initializing pause logic using animationPlayState.");

    const words = Array.from(document.querySelectorAll('.initial-content .word'));
    const wordCount = words.length;
    const pauseDurationMs = 3000; // ポーズ時間 (ms)

    if (wordCount === 0) return;

    words.forEach((word, index) => {
        // 各単語の animationiteration イベントを監視
        word.addEventListener('animationiteration', (event) => {
            // console.log(`Iteration ended for word ${index}: ${word.textContent.trim()}`);

            // イベント発生元が .slow クラスを持っているか確認
            if (word.classList.contains('slow')) {
                console.log(`Word ${index} is slow. Pausing its animation for ${pauseDurationMs}ms.`);
                word.style.animationPlayState = 'paused'; // アニメーションを一時停止

                // ポーズ時間後にアニメーションを再開するタイマーを設定
                setTimeout(() => {
                    console.log(`  Resuming animation for word ${index}.`);
                    word.style.animationPlayState = 'running'; // アニメーションを再開
                }, pauseDurationMs);
            }
        });
    });

    // --- 時間によるテキスト変更ロジック  ---
    function updateDynamicTexts() {
        const now = new Date();
        const currentHour = now.getHours();
        const currentMinutes = now.getMinutes();
    
        // --- '.changeName' 要素の更新 ---
        const nameElement = document.querySelector('.word.changeName');
        if (nameElement) {
            let nameDisplayText = "";
    
            if (currentHour >= 15 && currentHour < 16) { nameDisplayText = "名古屋Orchest-Lab"; }
            else if (currentHour >= 16 && currentHour < 17) { nameDisplayText = "化ける身"; }
            else if (currentHour >= 17 && currentHour < 18) { nameDisplayText = "Rishao"; }
            // ↓↓↓ 18:00 から 18:14 の条件 ↓↓↓
            else if (currentHour === 18 && currentMinutes < 15) { nameDisplayText = "the MusicVideo"; }
            // ↓↓↓ 18:15 以降の条件 (18時かつ15分以上、または19時以降) ↓↓↓
            else if (currentHour > 18 || (currentHour === 18 && currentMinutes >= 15)) { nameDisplayText = "thank you"; }
    
            if (nameElement.textContent !== nameDisplayText) {
                nameElement.textContent = nameDisplayText;
                console.log("Updated changeName text to:", nameDisplayText);
            }
        }
    
        // --- '.changeComingText' 要素の更新 ---
        const comingElement = document.querySelector('.word.changeComingText');
        if (comingElement) {
            let comingDisplayText = "Coming up next.."; // デフォルト
    
            // 18時以降の条件を追加
            if (currentHour === 18 && currentMinutes < 15) {
                comingDisplayText = "about";
            }
            else if (currentHour > 18 || (currentHour === 18 && currentMinutes >= 15)) {
                comingDisplayText = ""; 
            }
            // それ以外の時間は "Coming up next.." になる
    
            if (comingElement.textContent !== comingDisplayText) {
                comingElement.textContent = comingDisplayText;
                console.log("Updated changeComingText text to:", comingDisplayText);
            }
        } else {
            // console.warn("Element with class '.word.changeComingText' not found."); // 必要なら警告表示
        }
    }
    updateDynamicTexts();
    setInterval(updateDynamicTexts, 60000);

});