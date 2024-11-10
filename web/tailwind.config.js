/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: ({ colors }) => ({
          primary : colors.indigo,
          danger : colors.rose,
          warning : colors.yellow,
          success : colors.lime,
          info : colors.blue,
          gray : colors.zinc,
      }),
  }
  },
}

